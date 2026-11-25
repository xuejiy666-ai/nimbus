int baidu_try(const char *apkPath, const char *outDir, UnpackResult *r)
{
    ZipReader z;
    if (zip_open(apkPath, &z) != 0) return 0;

    StrList jars, mds;
    sl_init(&jars);
    sl_init(&mds);
    int *soIdx = NULL;
    int nb = 0;
    for (int i = 0; i < z.n; i++) {
        const char *nm = z.e[i].name;
        if (!bd_hit_name(nm)) continue;
        if (ends_with_ci(nm, ".jar")) { sl_add(&jars, nm); continue; }
        if (ends_with_ci(nm, ".md")) { sl_add(&mds, nm); continue; }
        if (ends_with_ci(nm, ".dex")) continue;
        
        if (ends_with_ci(nm, ".so") || strstr(nm, "libbaiduprotect_x86_64")
            || strstr(nm, "libbaiduprotect_")) {
            soIdx = (int *) xrealloc(soIdx, sizeof(int) * (size_t) (nb + 1));
            soIdx[nb++] = i;
        }
    }
    
    for (int i = 0; i < nb; i++) {
        if (strstr(z.e[soIdx[i]].name, "arm64-v8a")) {
            int t = soIdx[0]; soIdx[0] = soIdx[i]; soIdx[i] = t;
            break;
        }
    }
    if (jars.n == 0 || mds.n == 0) {
        free(soIdx);
        sl_clear(&jars);
        sl_clear(&mds);
        zip_close(&z);
        return 0;
    }

        
    Buf manifest = {NULL, 0};
    {
        ZipEntry *me0 = zip_find(&z, "AndroidManifest.xml");
        if (me0) zip_read(&z, me0, &manifest);
    }
    char *pkg = manifest.p ? axml_read_package(manifest.p, manifest.len) : NULL;
    char *entry = NULL;
    {
        ZipEntry *se0 = zip_find(&z, "classes.dex");
        if (se0) {
            Buf sd0 = {NULL, 0};
            if (zip_read(&z, se0, &sd0) == 0 && sd0.p) {
                entry = bd_dex_appname(sd0.p, sd0.len);
                free(sd0.p);
            }
        }
    }
    char *dot = bd_dotted(entry);
        if (pkg) {
                r->pkg = xstrdup(pkg);
    }
    {
        char nm2[256];
        const char *bs = strrchr(apkPath, '/');
        snprintf(nm2, sizeof(nm2), "%s", bs ? bs + 1 : apkPath);
        char *cut = strrchr(nm2, '.');
        if (cut && cut != nm2) *cut = 0;
        static char bdOut[4096];
        snprintf(bdOut, sizeof(bdOut), "%s/%s", outDir, (pkg && pkg[0]) ? pkg : nm2);
        mkdir_p(bdOut);
        outDir = bdOut;
        r->outDir = xstrdup(bdOut);
    }

    char alg[17], mode[17];
    memset(alg, 0, sizeof(alg));
    memset(mode, 0, sizeof(mode));
    int found = 0;
    for (int i = 0; i < nb && !found; i++) {
        Buf b = {NULL, 0};
        if (zip_read(&z, &z.e[soIdx[i]], &b) != 0) continue;
        if (!(b.len > 4 && b.p[0] == 0x7f && b.p[1] == 'E' && b.p[2] == 'L' && b.p[3] == 'F')) {
            free(b.p);
            continue;
        }
        BdHexList lx;
        memset(&lx, 0, sizeof(lx));
        bd_hex_scan(b.p, b.len, &lx);
        if (lx.n > 0) found = bd_match(b.p, b.len, &lx, &mds, &z, alg, mode);
        free(lx.v);
        if (!found) found = bd_xor_crack(&b, 1, &mds, &z, alg, mode);
        free(b.p);
    }
    free(soIdx);
    if (!found) {
                r->tail = xstrdup("");
        sl_clear(&jars);
        sl_clear(&mds);
        zip_close(&z);
        return 1;
    }

    uint8_t mat[16];
    int have = 0;
    for (int i = 0; i < mds.n && !have; i++) {
        ZipEntry *e = zip_find(&z, mds.v[i]);
        if (!e) continue;
        Buf md = {NULL, 0};
        if (zip_read(&z, e, &md) != 0) continue;
        uint8_t key[16], nonce[12];
        bd_kdf((const uint8_t *) alg, strlen(alg),
               (const uint8_t *) mds.v[i], strlen(mds.v[i]), mode, key, nonce);
        bd_ctr(md.p, md.len, key, nonce, md.len);
        if (md.len > 0x24 && memcmp(md.p, "md01", 4) == 0) {
            memcpy(mat, md.p + 0x14, 16);
            have = 1;
        }
        free(md.p);
    }

    if (!have) {
                r->tail = xstrdup("");
        sl_clear(&jars);
        sl_clear(&mds);
        zip_close(&z);
        return 1;
    }

    uint8_t key[16], nonce[12];
    bd_kdf((const uint8_t *) alg, strlen(alg), mat, 16, mode, key, nonce);

    r->keyHex = hex_of(key, 16);

    
    BdOrd *ord = NULL;
    int no = 0;
    for (int i = 0; i < z.n; i++) {
        const char *nm = z.e[i].name;
        if (!is_dex_name(nm)) continue;
        int idx = 1;
        if (nm[7] >= '0' && nm[7] <= '9') idx = atoi(nm + 7);
        ord = (BdOrd *) xrealloc(ord, sizeof(BdOrd) * (size_t) (no + 1));
        ord[no].idx = idx;
        ord[no].slot = i;
        no++;
    }
    qsort(ord, (size_t) no, sizeof(BdOrd), bd_cmp_ord);

    int cap = no + jars.n;
    Buf *outs = (Buf *) xcalloc((size_t) (cap > 0 ? cap : 1), sizeof(Buf));
    int nouts = 0;
    for (int i = 0; i < no; i++) {
        Buf b = {NULL, 0};
        if (zip_read(&z, &z.e[ord[i].slot], &b) == 0) outs[nouts++] = b;
    }
    free(ord);

    for (int i = 0; i < jars.n; i++) {
        ZipEntry *e = zip_find(&z, jars.v[i]);
        if (!e) continue;
        Buf raw = {NULL, 0};
        if (zip_read(&z, e, &raw) != 0) continue;
        Buf dec = {NULL, 0};
        size_t lims[3] = {256, 512, raw.len};
        for (int t = 0; t < 3; t++) {
            Buf tmp = {NULL, 0};
            tmp.p = (uint8_t *) xmalloc(raw.len);
            memcpy(tmp.p, raw.p, raw.len);
            tmp.len = raw.len;
            bd_ctr(tmp.p, tmp.len, key, nonce, lims[t]);
            if (bd_dinflate(tmp.p, tmp.len, &dec) == 0 && dec.len >= 4
                && memcmp(dec.p, "dex\n", 4) == 0) {
                free(tmp.p);
                break;
            }
            free(tmp.p);
            free(dec.p);
            dec.p = NULL;
            dec.len = 0;
        }
        free(raw.p);
        if (dec.p && nouts < cap) outs[nouts++] = dec;
        else free(dec.p);
    }

    if (nouts == 0) {
                r->tail = xstrdup("");
        free(pkg);
        free(manifest.p);
        sl_clear(&jars);
        sl_clear(&mds);
        zip_close(&z);
        return 1;
    }

    if (!entry && nouts > 0) {
        entry = bd_dex_appname(outs[0].p, outs[0].len);
        dot = bd_dotted(entry);
    }
            
    for (int i = 0; i < nouts; i++) {
        char p[4096];
        if (i == 0) snprintf(p, sizeof(p), "%s/classes.dex", outDir);
        else snprintf(p, sizeof(p), "%s/classes%d.dex", outDir, i + 1);
        FILE *f = fopen(p, "wb");
        if (f) {
            if (outs[i].len) fwrite(outs[i].p, 1, outs[i].len, f);
            fclose(f);
                    }
    }
    r->dexCount = nouts;
    r->ok = 1;

    for (int i = 0; i < nouts; i++) free(outs[i].p);
    free(outs);
    free(dot);
    free(entry);
    free(pkg);
    free(manifest.p);
    sl_clear(&jars);
    sl_clear(&mds);
    zip_close(&z);
    return 1;
}
