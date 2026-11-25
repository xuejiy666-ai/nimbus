void *dex_job_run(void *arg)
{
    DexJob *j = (DexJob *) arg;
    if (!j->src || j->slen < 0x70 || memcmp(j->src, "dex\n", 4) != 0) return NULL;
    j->plain.p = (uint8_t *) xmalloc(j->slen);
    memcpy(j->plain.p, j->src, j->slen);
    j->plain.len = j->slen;
    if (write_file(j->path, j->plain.p, j->plain.len) != 0) return NULL;
    j->crc = (uint32_t) crc32(0L, j->plain.p, (uInt) j->plain.len);
    j->ok = 1;
    return NULL;
}
