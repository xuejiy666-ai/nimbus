int ep_inflate_w(const uint8_t *d, size_t n, int wbits, Buf *out)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (inflateInit2(&zs, wbits) != Z_OK) return -1;
    size_t cap = n * 8 + 65536;
    uint8_t *buf = (uint8_t *) xmalloc(cap);
    zs.next_in = (Bytef *) d;
    zs.avail_in = (uInt) n;
    size_t have = 0;
    for (;;) {
        while (have + 65536 > cap) { cap *= 2; buf = (uint8_t *) xrealloc(buf, cap); }
        zs.next_out = buf + have;
        zs.avail_out = (uInt) (cap - have);
        size_t before = have;
        int rc = inflate(&zs, Z_NO_FLUSH);
        have = (size_t) (zs.next_out - buf);
        if (rc == Z_STREAM_END) break;
        if (rc == Z_OK) continue;
        if (rc == Z_BUF_ERROR && zs.avail_in == 0 && have == before) break;  
        inflateEnd(&zs);
        free(buf);
        return -1;
    }
    inflateEnd(&zs);
    out->p = buf;
    out->len = have;
    return 0;
}
