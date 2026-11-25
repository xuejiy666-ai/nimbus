int zip_writer_add_raw(ZipWriter *w, const char *name, int method, uint32_t crc,
                       uint64_t csize, uint64_t usize, const uint8_t *data, size_t len)
{
    if (!w->f) return -1;
    size_t nlen = strlen(name);
    if (nlen > 0xffff) return -1;
    uint64_t off = w->offset;

    Vec h;
    vec_init(&h);
    vec_put32(&h, 0x04034b50u);
    vec_put16(&h, 20);
    vec_put16(&h, 0x0800);
    vec_put16(&h, (uint32_t) method);
    vec_put16(&h, 0); vec_put16(&h, 0);
    vec_put32(&h, crc);
    vec_put32(&h, (uint32_t) csize);
    vec_put32(&h, (uint32_t) usize);
    vec_put16(&h, (uint32_t) nlen);
    vec_put16(&h, 0);
    vec_write(&h, name, nlen);
    int ok = fwrite(h.p, 1, h.len, w->f) == h.len;
    free(h.p);
    if (ok && len) ok = fwrite(data, 1, len, w->f) == len;
    if (!ok) return -1;
    w->offset = off + 30 + nlen + csize;

    vec_put32(&w->cd, 0x02014b50u);
    vec_put16(&w->cd, 20);
    vec_put16(&w->cd, 20);
    vec_put16(&w->cd, 0x0800);
    vec_put16(&w->cd, (uint32_t) method);
    vec_put16(&w->cd, 0); vec_put16(&w->cd, 0);
    vec_put32(&w->cd, crc);
    vec_put32(&w->cd, (uint32_t) csize);
    vec_put32(&w->cd, (uint32_t) usize);
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
