int ep_payload_dexes(const uint8_t *payload, size_t pn, const char *workDir,
                            EpDex **out, int *n)
{
    char tmp[4096];
    snprintf(tmp, sizeof(tmp), "%s/.epic_payload.zip", workDir);
    FILE *f = fopen(tmp, "wb");
    if (!f) return -1;
    if (pn) fwrite(payload, 1, pn, f);
    fclose(f);
    ZipReader z;
    if (zip_open(tmp, &z) != 0) { remove(tmp); return -1; }
    EpDex *v = NULL;
    int nv = 0;
    for (int i = 0; i < z.n; i++) {
        const char *nm = z.e[i].name;
        const char *base = strrchr(nm, '/');
        base = base ? base + 1 : nm;
        if (!is_dex_name(base)) continue;
        Buf b = {NULL, 0};
        if (zip_read(&z, &z.e[i], &b) != 0) continue;
        v = (EpDex *) xrealloc(v, sizeof(EpDex) * (size_t) (nv + 1));
        v[nv].name = xstrdup(base);
        v[nv].data = b;
        nv++;
    }
    zip_close(&z);
    remove(tmp);
    if (!nv) { free(v); return -1; }
    *out = v;
    *n = nv;
    return 0;
}
