int zip_read(ZipReader *z, ZipEntry *e, Buf *out)
{
    out->p = NULL; out->len = 0;
    if (!z || !z->f || !e) return -1;
    uint8_t lh[30];
    if (read_at(z->f, e->lho, lh, sizeof(lh)) != 0) return -1;
    if (!(lh[0] == 'P' && lh[1] == 'K' && lh[2] == 3 && lh[3] == 4)) return -1;
    uint16_t nlen = (uint16_t) (lh[26] | (lh[27] << 8));
    uint16_t elen = (uint16_t) (lh[28] | (lh[29] << 8));
    uint64_t doff = e->lho + 30 + nlen + elen;

    if (e->usize > (uint64_t) 1 << 31) return -1;
    uint8_t *buf = xmalloc((size_t) e->usize + 1);
    if (e->method == 0) {
        if (read_at(z->f, doff, buf, (size_t) e->usize) != 0) { free(buf); return -1; }
    } else if (e->method == 8) {
        if (e->csize > (uint64_t) 1 << 31) { free(buf); return -1; }
        uint8_t *cin = xmalloc((size_t) e->csize + 1);
        if (read_at(z->f, doff, cin, (size_t) e->csize) != 0) { free(cin); free(buf); return -1; }
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        if (inflateInit2(&zs, -15) != Z_OK) { free(cin); free(buf); return -1; }
        zs.next_in = cin;
        zs.avail_in = (uInt) e->csize;
        zs.next_out = buf;
        zs.avail_out = (uInt) e->usize;
        int rc = inflate(&zs, Z_FINISH);
        inflateEnd(&zs);
        free(cin);
        if (rc != Z_STREAM_END && rc != Z_BUF_ERROR) { free(buf); return -1; }
    } else {
        free(buf);
        return -1;
    }
    out->p = buf;
    out->len = (size_t) e->usize;
    return 0;
}
