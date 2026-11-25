int zip_raw_read(ZipReader *z, ZipEntry *e, Buf *out)
{
    out->p = NULL; out->len = 0;
    if (!z->f || !e) return -1;
    uint8_t lh[30];
    if (fseek(z->f, (long) e->lho, SEEK_SET) != 0) return -1;
    if (fread(lh, 1, 30, z->f) != 30) return -1;
    if (rd32(lh, 30, 0) != 0x04034b50u) return -1;
    uint64_t off = e->lho + 30 + rd16(lh, 30, 26) + rd16(lh, 30, 28);
    if (e->csize > (uint64_t) 1 << 32) return -1;
    uint8_t *p = xmalloc((size_t) e->csize + 1);
    if (fseek(z->f, (long) off, SEEK_SET) != 0) { free(p); return -1; }
    if (e->csize && fread(p, 1, (size_t) e->csize, z->f) != (size_t) e->csize) {
        free(p);
        return -1;
    }
    out->p = p;
    out->len = (size_t) e->csize;
    return 0;
}
