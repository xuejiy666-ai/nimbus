int bd_dinflate(const uint8_t *d, size_t n, Buf *out)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (inflateInit2(&zs, 47) != Z_OK) return -1;
    size_t cap = n * 4 + 4096;
    uint8_t *p = (uint8_t *) xmalloc(cap);
    zs.next_in = (Bytef *) d;
    zs.avail_in = (uInt) n;
    zs.next_out = p;
    zs.avail_out = (uInt) cap;
    int rc = inflate(&zs, Z_FINISH);
    if (rc != Z_STREAM_END) { inflateEnd(&zs); free(p); return -1; }
    out->p = p;
    out->len = cap - zs.avail_out;
    inflateEnd(&zs);
    return 0;
}
