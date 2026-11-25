int zip_writer_close(ZipWriter *w)
{
    if (!w->f) return -1;
    uint64_t cd_off = w->offset;
    if (w->cd.len && fwrite(w->cd.p, 1, w->cd.len, w->f) != w->cd.len) return -1;
    uint64_t size = cd_off + w->cd.len;

    Vec e;
    vec_init(&e);
    vec_put32(&e, 0x06054b50u);
    vec_put16(&e, 0);
    vec_put16(&e, 0);
    vec_put16(&e, (uint32_t) w->count);
    vec_put16(&e, (uint32_t) w->count);
    vec_put32(&e, (uint32_t) w->cd.len);
    vec_put32(&e, (uint32_t) cd_off);
    vec_put16(&e, 0);
    fwrite(e.p, 1, e.len, w->f);
    free(e.p);
    vec_free(&w->cd);
    fclose(w->f);
    w->f = NULL;
    return (int) size;
}
