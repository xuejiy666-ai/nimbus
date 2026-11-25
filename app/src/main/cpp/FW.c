int epic2_try(const char *apkPath, const char *outDir, UnpackResult *r)
{
    ZipReader z;
    if (zip_open(apkPath, &z) != 0) return 0;

    char *json = NULL;
    for (int i = 0; i < z.n && !json; i++) {
        const char *nm = z.e[i].name;
        if (!(e2_ends(nm, "libEPIC.so") || e2_ends(nm, "libEP.so"))) continue;
        Buf b = {NULL, 0};
        if (zip_read(&z, &z.e[i], &b) != 0) continue;
        Buf sec = e2_elf_section(b.p, b.len, ".ArmEpic");
        free(b.p);
        if (sec.p && sec.len > 16) {
            uint8_t *pl = (uint8_t *) xmalloc(sec.len - 15);
            rc4_crypt(sec.p, 16, sec.p + 16, pl, sec.len - 16);
            pl[sec.len - 16] = 0;
            json = e2_extract_json((char *) pl);
            free(pl);
        }
        free(sec.p);
        if (json && !strstr(json, "vm_data_name") && !strstr(json, "dex_dir")) {
            free(json);
            json = NULL;
        }
    }
    for (int i = 0; i < z.n && !json; i++) {
        const char *nm = z.e[i].name;
        if (!e2_ends(nm, "config.json")) continue;
        Buf b = {NULL, 0};
        if (zip_read(&z, &z.e[i], &b) != 0) continue;
        char *txt = (char *) xmalloc(b.len + 1);
        memcpy(txt, b.p, b.len);
        txt[b.len] = 0;
        free(b.p);
        if (strstr(txt, "vm_data_name") || strstr(txt, "dex_protection")) json = e2_extract_json(txt);
        free(txt);
    }
    if (!json) { zip_close(&z); return 0; }

    E2Cfg c;
    e2_cfg_parse(json, &c);
    free(json);

    char dir[256];
    snprintf(dir, sizeof(dir), "%s", c.dex_dir ? c.dex_dir : "EPIC");
    for (size_t i = 0; i < strlen(dir); i++)
        if (dir[i] == '/') dir[i] = 0;
    if (!dir[0]) snprintf(dir, sizeof(dir), "EPIC");
    char dirSlash[300], dirMid[300];
    snprintf(dirSlash, sizeof(dirSlash), "%s/", dir);
    snprintf(dirMid, sizeof(dirMid), "/%s/", dir);

    int useRc4 = c.dex_rc4 && *c.dex_rc4;
    if (!useRc4 && !c.dex_xor) { zip_close(&z); e2_cfg_free(&c); return 0; }

    Buf *dx = NULL;
    int nd = 0;
    for (int i = 0; i < z.n; i++) {
        const char *nm = z.e[i].name;
        if (e2_ends(nm, "/")) continue;
        if (!(e2_starts(nm, dirSlash) || strstr(nm, dirMid))) continue;
        Buf b = {NULL, 0};
        if (zip_read(&z, &z.e[i], &b) != 0) continue;
        Buf pl = e2_dec_buf(b.p, b.len, useRc4 ? 1 : 0, c.dex_xor, c.dex_rc4);
        free(b.p);
        if (!pl.p || pl.len < 8 || memcmp(pl.p, "dex\n", 4) != 0) { free(pl.p); continue; }
        dx = (Buf *) xrealloc(dx, sizeof(Buf) * (size_t)(nd + 1));
        dx[nd++] = pl;
    }
    if (!nd) {
        free(dx);
        e2_cfg_free(&c);
        zip_close(&z);
        return 1;
    }
    {
        int real = 0;
        for (int i = 0; i < nd; i++) if (!e2_is_shell_dex(dx[i].p, dx[i].len)) real++;
        if (real > 0) {
            int k = 0;
            for (int i = 0; i < nd; i++) {
                if (e2_is_shell_dex(dx[i].p, dx[i].len)) free(dx[i].p);
                else dx[k++] = dx[i];
            }
            nd = k;
        }
    }
    qsort(dx, (size_t) nd, sizeof(Buf), e2_cmp_size);

    char *pkg = NULL;
    Buf man = {NULL, 0};
    ZipEntry *me = zip_find(&z, "AndroidManifest.xml");
    if (me) {
        zip_read(&z, me, &man);
        if (man.p) pkg = axml_read_package(man.p, man.len);
    }

    char work[4096];
    sc_out_dir(work, sizeof(work), outDir, apkPath, pkg, "epicv2", NULL);
    free(r->outDir);
    r->outDir = xstrdup(work);

    for (int i = 0; i < nd; i++) {
        char p[4300];
        if (i == 0) snprintf(p, sizeof(p), "%s/classes.dex", work);
        else snprintf(p, sizeof(p), "%s/classes%d.dex", work, i + 1);
        FILE *f = fopen(p, "wb");
        if (f) { fwrite(dx[i].p, 1, dx[i].len, f); fclose(f); }
    }

    if (pkg) r->pkg = xstrdup(pkg);
    r->dexCount = nd;
    r->ok = 1;

    free(pkg);
    free(man.p);
    for (int i = 0; i < nd; i++) free(dx[i].p);
    free(dx);
    e2_cfg_free(&c);
    zip_close(&z);
    return 1;
}
