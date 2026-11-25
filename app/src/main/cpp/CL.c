Buf deflate_buf(const uint8_t *d, size_t n)
{
    Buf out = {NULL, 0};
    uLong bound = compressBound((uLong) n) + 64;
    uint8_t *p = (uint8_t *) xmalloc((size_t) bound);
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (deflateInit2(&zs, 1, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        free(p);
        return out;
    }
    zs.next_in = (Bytef *) d;
    zs.avail_in = (uInt) n;
    zs.next_out = p;
    zs.avail_out = (uInt) bound;
    int rc = deflate(&zs, Z_FINISH);
    out.len = (size_t) zs.total_out;
    deflateEnd(&zs);
    if (rc != Z_STREAM_END) {
        free(p);
        out.len = 0;
        return out;
    }
    out.p = p;
    return out;
}
