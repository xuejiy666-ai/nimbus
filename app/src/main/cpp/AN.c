int zip_writer_add(ZipWriter *w, const char *name, const uint8_t *data, size_t len)
{
    if (!w->f) return -1;
    size_t nlen = strlen(name);
    if (nlen > 0xffff) return -1;

    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, data, (uInt) len);

    
    uLong bound = compressBound((uLong) len) + 64;
    uint8_t *comp = xmalloc((size_t) bound);
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (deflateInit2(&zs, 1, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        free(comp);
        return -1;
    }
    zs.next_in = (Bytef *) data;
    zs.avail_in = (uInt) len;
    zs.next_out = comp;
    zs.avail_out = (uInt) bound;
    int rc = deflate(&zs, Z_FINISH);
    size_t clen = (size_t) zs.total_out;
    deflateEnd(&zs);
    if (rc != Z_STREAM_END) { free(comp); return -1; }

    uint64_t off = w->offset;
    if (len == 0) { clen = 0; }

    Vec h;
    vec_init(&h);
    vec_put32(&h, 0x04034b50u);
    vec_put16(&h, 20);
    vec_put16(&h, 0x0800);           
    vec_put16(&h, 8);
    vec_put16(&h, 0); vec_put16(&h, 0);   
    vec_put32(&h, (uint32_t) crc);
    vec_put32(&h, (uint32_t) clen);
    vec_put32(&h, (uint32_t) len);
    vec_put16(&h, (uint32_t) nlen);
    vec_put16(&h, 0);
    vec_write(&h, name, nlen);
    fwrite(h.p, 1, h.len, w->f);
    free(h.p);

    if (clen && fwrite(comp, 1, clen, w->f) != clen) { free(comp); return -1; }
    w->offset = off + 30 + nlen + clen;
    free(comp);

    
    vec_put32(&w->cd, 0x02014b50u);
    vec_put16(&w->cd, 20);
    vec_put16(&w->cd, 20);
    vec_put16(&w->cd, 0x0800);
    vec_put16(&w->cd, 8);
    vec_put16(&w->cd, 0); vec_put16(&w->cd, 0);
    vec_put32(&w->cd, (uint32_t) crc);
    vec_put32(&w->cd, (uint32_t) clen);
    vec_put32(&w->cd, (uint32_t) len);
    vec_put16(&w->cd, (uint32_t) nlen);
    vec_put16(&w->cd, 0);
    vec_put16(&w->cd, 0);
    vec_put16(&w->cd, 0);
    vec_put16(&w->cd, 0);
    vec_put32(&w->cd, 0);
    vec_put32(&w->cd, (uint32_t) off);
    vec_write(&w->cd, name, nlen);
    w->count++;
    return 0;
}
