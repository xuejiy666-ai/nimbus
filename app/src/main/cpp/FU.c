Buf e2_arsc_repair(const uint8_t *b, size_t n, int method, const char *xork, const char *rc4k)
{
    Buf fail = {NULL, 0};
    if (n < 12 || rd16(b, n, 0) != 2) return fail;
    uint32_t ths = rd16(b, n, 2);
    if (ths + 8 > n || rd16(b, n, ths) != 1) return fail;
    uint32_t hs = rd16(b, n, ths + 2);
    uint32_t chunkSize = rd32(b, n, ths + 4);
    if (hs < 28 || !chunkSize || ths + chunkSize > n) return fail;
    uint32_t cnt = rd32(b, n, ths + 8);
    uint32_t styleCount = rd32(b, n, ths + 12);
    uint32_t flags = rd32(b, n, ths + 16);
    uint32_t stringsStart = rd32(b, n, ths + 20);
    uint32_t stylesStart = rd32(b, n, ths + 24);
    if (!cnt || cnt > 1000000) return fail;
    int utf8 = (flags & 0x100) != 0;
    uint32_t *offs = (uint32_t *) xmalloc(sizeof(uint32_t) * cnt);
    for (uint32_t i = 0; i < cnt; i++) offs[i] = rd32(b, n, ths + hs + (size_t) i * 4);
    size_t dataBase = ths + stringsStart;
    size_t dataEnd = ths + chunkSize;
    Vec data;
    vec_init(&data);
    uint32_t *newoff = (uint32_t *) xmalloc(sizeof(uint32_t) * cnt);
    int changed = 0;
    for (uint32_t i = 0; i < cnt; i++) {
        size_t p = dataBase + offs[i];
        if (p >= dataEnd) { free(offs); free(newoff); vec_free(&data); return fail; }
        char *old = decode_at(b, n, p, utf8);
        char *pl = e2_decode_ep(old, method, xork, rc4k);
        char *use = pl ? pl : old;
        if (pl) changed++;
        Buf enc = encode_string(use, utf8);
        newoff[i] = (uint32_t) data.len;
        vec_write(&data, enc.p, enc.len);
        free(enc.p);
        free(old);
        free(pl);
    }
    if (!changed) { free(offs); free(newoff); vec_free(&data); return fail; }
    while (data.len % 4) vec_put8(&data, 0);

    Buf styleData = {NULL, 0};
    if (styleCount > 0 && stylesStart) {
        size_t st = ths + stylesStart, en = ths + chunkSize;
        if (st < en && en <= n) {
            styleData.len = en - st;
            styleData.p = (uint8_t *) xmalloc(styleData.len);
            memcpy(styleData.p, b + st, styleData.len);
        }
    }
    uint32_t newStringsStart = hs + cnt * 4 + styleCount * 4;
    uint32_t newStylesStart = styleCount > 0 ? (uint32_t)(newStringsStart + data.len) : 0;
    uint32_t newChunk = styleCount > 0 ? (uint32_t)(newStylesStart + styleData.len)
                                      : (uint32_t)(newStringsStart + data.len);
    Vec pool;
    vec_init(&pool);
    vec_put16(&pool, 1);
    vec_put16(&pool, hs);
    vec_put32(&pool, newChunk);
    vec_put32(&pool, cnt);
    vec_put32(&pool, styleCount);
    vec_put32(&pool, flags);
    vec_put32(&pool, newStringsStart);
    vec_put32(&pool, newStylesStart);
    for (uint32_t i = 0; i < cnt; i++) vec_put32(&pool, newoff[i]);
    for (uint32_t i = 0; i < styleCount; i++)
        vec_put32(&pool, rd32(b, n, ths + hs + cnt * 4 + (size_t) i * 4));
    vec_write(&pool, data.p, data.len);
    if (styleData.p) vec_write(&pool, styleData.p, styleData.len);

    Vec out;
    vec_init(&out);
    vec_write(&out, b, ths);
    vec_write(&out, pool.p, pool.len);
    if (ths + chunkSize < n) vec_write(&out, b + ths + chunkSize, n - (ths + chunkSize));
    size_t total = out.len;
    out.p[4] = (uint8_t)(total & 0xff);
    out.p[5] = (uint8_t)((total >> 8) & 0xff);
    out.p[6] = (uint8_t)((total >> 16) & 0xff);
    out.p[7] = (uint8_t)((total >> 24) & 0xff);
    free(offs); free(newoff); vec_free(&data); free(styleData.p); vec_free(&pool);
    Buf r = {out.p, out.len};
    return r;
}
