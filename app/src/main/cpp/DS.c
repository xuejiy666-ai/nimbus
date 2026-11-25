int ep_cbc_dec(const uint8_t *ct, size_t n, const uint8_t key[16],
                      const uint8_t iv[16], Buf *out)
{
    if (!n || (n & 15)) return -1;
    ep_init();
    uint8_t rk[176];
    bd_expand(key, rk);
    uint8_t *o = (uint8_t *) xmalloc(n);
    uint8_t prev[16], blk[16];
    memcpy(prev, iv, 16);
    for (size_t off = 0; off < n; off += 16) {
        ep_dec_block(ct + off, rk, blk);
        for (int i = 0; i < 16; i++) o[off + i] = (uint8_t) (blk[i] ^ prev[i]);
        memcpy(prev, ct + off, 16);
    }
    int pad = o[n - 1];
    if (pad < 1 || pad > 16) { free(o); return -1; }
    for (int i = 0; i < pad; i++)
        if (o[n - 1 - i] != (uint8_t) pad) { free(o); return -1; }
    out->p = o;
    out->len = n - (size_t) pad;
    return 0;
}
