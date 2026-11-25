int dpt_try(const char *apkPath, const char *outDir, UnpackResult *r)
{
    DptCtx c;
    int st = dpt_load(apkPath, &c);
    if (st == 0) { dpt_ctx_free(&c); return 0; }
    if (st == 2) { dpt_ctx_free(&c); return 1; }
    if (!dpt_gather(&c, outDir)) { dpt_ctx_free(&c); return 1; }

    char hex[33] = {0};
    for (int i = 0; i < 16; i++) sprintf(hex + i * 2, "%02x", c.ak[0][i]);
    r->keyHex = xstrdup(hex);
    r->pkg = xstrdup(c.pkg);

    char work[4096];
    sc_out_dir(work, sizeof(work), outDir, apkPath, c.pkg, "", NULL);
    r->outDir = xstrdup(work);

    for (int i = 0; i < c.nd; i++) {
        char p[4096];
        if (i == 0) snprintf(p, sizeof(p), "%s/classes.dex", work);
        else snprintf(p, sizeof(p), "%s/classes%d.dex", work, i + 1);
        FILE *f = fopen(p, "wb");
        if (f) { fwrite(c.dexes[i].p, 1, c.dexes[i].len, f); fclose(f); }
    }
    r->dexCount = c.nd;
    r->ok = 1;

    dpt_ctx_free(&c);
    return 1;
}
