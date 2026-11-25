int zip_copy_raw(ZipReader *z, ZipEntry *e, ZipWriter *w)
{
    if (!z->f || !w->f || !e) return -1;
    uint8_t lh[30];
    if (fseek(z->f, (long) e->lho, SEEK_SET) != 0) return -1;
    if (fread(lh, 1, 30, z->f) != 30) return -1;
    if (rd32(lh, 30, 0) != 0x04034b50u) return -1;
    uint64_t off = e->lho + 30 + rd16(lh, 30, 26) + rd16(lh, 30, 28);
    size_t nlen = strlen(e->name);
    if (nlen > 0xffff) return -1;
    uint64_t woff = w->offset;

    uint8_t h[30 + 512];
    if (nlen > 512) return -1;
    memset(h, 0, 30);
    wr32(h, sizeof(h), 0, 0x04034b50u);
    wr16(h, sizeof(h), 4, 20);
    wr16(h, sizeof(h), 6, 0x0800);
    wr16(h, sizeof(h), 8, (uint32_t) e->method);
    wr32(h, sizeof(h), 14, e->crc);
    wr32(h, sizeof(h), 18, (uint32_t) e->csize);
    wr32(h, sizeof(h), 22, (uint32_t) e->usize);
    wr16(h, sizeof(h), 26, (uint32_t) nlen);
    memcpy(h + 30, e->name, nlen);
    if (fwrite(h, 1, 30 + nlen, w->f) != 30 + nlen) return -1;

    if (fseek(z->f, (long) off, SEEK_SET) != 0) return -1;
    uint8_t buf[1 << 16];
    uint64_t left = e->csize;
    while (left) {
        size_t chunk = left > sizeof(buf) ? sizeof(buf) : (size_t) left;
        if (fread(buf, 1, chunk, z->f) != chunk) return -1;
        if (fwrite(buf, 1, chunk, w->f) != chunk) return -1;
        left -= chunk;
    }
    w->offset = woff + 30 + nlen + e->csize;

    vec_put32(&w->cd, 0x02014b50u);
    vec_put16(&w->cd, 20);
    vec_put16(&w->cd, 20);
    vec_put16(&w->cd, 0x0800);
    vec_put16(&w->cd, (uint32_t) e->method);
    vec_put16(&w->cd, 0); vec_put16(&w->cd, 0);
    vec_put32(&w->cd, e->crc);
    vec_put32(&w->cd, (uint32_t) e->csize);
    vec_put32(&w->cd, (uint32_t) e->usize);
    vec_put16(&w->cd, (uint32_t) nlen);
    vec_put16(&w->cd, 0);
    vec_put16(&w->cd, 0);
    vec_put16(&w->cd, 0);
    vec_put16(&w->cd, 0);
    vec_put32(&w->cd, 0);
    vec_put32(&w->cd, (uint32_t) woff);
    vec_write(&w->cd, e->name, nlen);
    w->count++;
    return 0;
}
