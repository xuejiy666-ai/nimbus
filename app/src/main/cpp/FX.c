int epic_try(const char *apkPath, const char *outDir, UnpackResult *r)
{
    ZipReader z;
    if (zip_open(apkPath, &z) != 0) return 0;

    int hasCfg = zip_find(&z, "assets/Epic_Config") != NULL;
    Buf cls0 = {NULL, 0};
    ZipEntry *ce = zip_find(&z, "classes.dex");
    if (ce) zip_read(&z, ce, &cls0);
    EpTail tk;
    int isExtract = hasCfg || (cls0.p && cls0.len >= 32 && ep_tail(cls0.p, cls0.len, &tk) == 0);
    int isAssets = 0;
    for (int i = 0; i < z.n && !isAssets; i++) {
        const char *nm = z.e[i].name;
        if (strncmp(nm, "assets/Epic/", 12) == 0 && ends_with_ci(nm, ".Epic") && strlen(nm) >= 5 && strcmp(nm + strlen(nm) - 5, ".Epic") == 0) isAssets = 1;
    }
    if (!isExtract && !isAssets) {
        free(cls0.p);
        zip_close(&z);
        return 0;
    }

    
    Buf manifest = {NULL, 0};
    ZipEntry *me = zip_find(&z, "AndroidManifest.xml");
    if (me) zip_read(&z, me, &manifest);
    char *pkg = manifest.p ? axml_read_package(manifest.p, manifest.len) : NULL;

    char work[4096];
    work[0] = 0;

    EpDex *dexes = NULL;
    int nd = 0;
    Buf cfg = {NULL, 0};
    EpBlock *blocks = NULL;
    int nblocks = 0;
    uint8_t key[16];
    memset(key, 0, sizeof(key));
    int haveKey = 0;
    char keyHex[33] = {0};
    char seedStr[16] = {0};
    char *entryCls = NULL, *factoryCls = NULL;
    const char *keyLib = NULL;
    StrList cands;
    sl_init(&cands);
    int okFlag = 0;

    if (isExtract) {
        Buf payload = {NULL, 0};
        int havePl = 0;
        if (ep_find_key(&z, cls0.p, cls0.len, key, &payload, &havePl) != 0 || !havePl) goto fail;
        haveKey = 1;
                ep_make_work(work, sizeof(work), outDir, apkPath, pkg, "");
        r->outDir = xstrdup(work);
        Buf eb = {NULL, 0};
        ZipEntry *ee = zip_find(&z, "assets/Epic_Config");
        if (!ee || zip_read(&z, ee, &eb) != 0) { free(payload.p); goto fail; }
        int rc2 = ep_epc2(eb.p, eb.len, key, &cfg, &blocks, &nblocks);
        free(eb.p);
        if (rc2 != 0) { free(payload.p); goto fail; }
        int pdr = ep_payload_dexes(payload.p, payload.len, work, &dexes, &nd);
        if (pdr != 0) {
            free(payload.p);
            goto fail;
        }
        free(payload.p);
        qsort(dexes, (size_t) nd, sizeof(EpDex), ep_cmp_dex);
        uint32_t insns = 0;

        (void) insns;
        (void) nblocks;
        for (int i = 0; i < 16; i++) sprintf(keyHex + i * 2, "%02x", key[i]);
        {
            char *a = ep_json_str((const char *) cfg.p, "\"app_name\"");
            char *b = ep_json_str((const char *) cfg.p, "\"acf_name\"");
            if (a && a[0]) sl_add(&cands, a);
            if (b && b[0]) sl_add(&cands, b);
            free(a);
            free(b);
        }
    } else {
        EpEnt *ents = NULL;
        int ne = 0;
        for (int i = 0; i < z.n; i++) {
            const char *nm = z.e[i].name;
            if (strncmp(nm, "assets/Epic/", 12) != 0) continue;
            if (!ends_with_ci(nm, ".epic")) continue;
            ents = (EpEnt *) xrealloc(ents, sizeof(EpEnt) * (size_t) (ne + 1));
            ents[ne].name = nm;
            ents[ne].size = z.e[i].usize;
            ents[ne].idx = i;
            ne++;
        }
        if (!ne) goto fail;
        qsort(ents, (size_t) ne, sizeof(EpEnt), ep_cmp_ent);

        EpEnt *libs = NULL;
        int nl = 0;
        for (int i = 0; i < z.n; i++) {
            if (!ends_with_ci(z.e[i].name, ".so")) continue;
            libs = (EpEnt *) xrealloc(libs, sizeof(EpEnt) * (size_t) (nl + 1));
            libs[nl].name = z.e[i].name;
            libs[nl].size = z.e[i].usize;
            libs[nl].idx = i;
            nl++;
        }
        qsort(libs, (size_t) nl, sizeof(EpEnt), ep_cmp_ent);

        Buf probe = {NULL, 0};
        zip_read(&z, &z.e[ents[0].idx], &probe);
        EpLcg chosen;
        memset(&chosen, 0, sizeof(chosen));
        int have = 0;
        for (int i = 0; i < nl && !have; i++) {
            Buf lb = {NULL, 0};
            if (zip_read(&z, &z.e[libs[i].idx], &lb) != 0) continue;
            EpLcg *cs = NULL;
            int nc = 0;
            int lrc = ep_lcg_find(lb.p, lb.len, &cs, &nc);
            if (lrc == 0) {
                for (int k = 0; k < nc && !have; k++) {
                    size_t pn = probe.len < 384 ? probe.len : 384;
                    uint8_t *t2 = (uint8_t *) xmalloc(pn ? pn : 1);
                    memcpy(t2, probe.p, pn);
                    ep_lcg_dec(t2, pn, cs[k].seed, cs[k].mult, cs[k].inc);
                    Buf q = {NULL, 0};
                    int quick = (ep_inflate_w(t2, pn, -15, &q) == 0);
                    free(q.p);
                    free(t2);
                    if (!quick) continue;
                    uint8_t *full = (uint8_t *) xmalloc(probe.len ? probe.len : 1);
                    memcpy(full, probe.p, probe.len);
                    ep_lcg_dec(full, probe.len, cs[k].seed, cs[k].mult, cs[k].inc);
                    Buf d = {NULL, 0};
                    int fr = ep_inflate_dex(full, probe.len, &d);
                    if (fr == 0) {
                        free(d.p);
                        chosen = cs[k];
                        keyLib = z.e[libs[i].idx].name;
                        have = 1;
                    }
                    free(full);
                }
            }
            free(cs);
            free(lb.p);
        }
        free(libs);
        free(probe.p);
        if (!have) { free(ents); goto fail; }
        int isAdv = (keyLib && strstr(keyLib, "Vm")) ? 1 : 0;
                ep_make_work(work, sizeof(work), outDir, apkPath, pkg,
                     isAdv ? "" : "");
        r->outDir = xstrdup(work);
        snprintf(seedStr, sizeof(seedStr), "%08x", chosen.seed);

        for (int i = 0; i < ne; i++) {
            Buf b = {NULL, 0};
            if (zip_read(&z, &z.e[ents[i].idx], &b) != 0) continue;
            ep_lcg_dec(b.p, b.len, chosen.seed, chosen.mult, chosen.inc);
            Buf d = {NULL, 0};
            if (ep_inflate_dex(b.p, b.len, &d) == 0) {
                dexes = (EpDex *) xrealloc(dexes, sizeof(EpDex) * (size_t) (nd + 1));
                dexes[nd].name = NULL;
                dexes[nd].data = d;
                nd++;
            }
            free(b.p);
        }
        free(ents);
        if (!nd) goto fail;
        qsort(dexes, (size_t) nd, sizeof(EpDex), ep_cmp_dexsize);
    }

    for (int i = 0; i < nd; i++) {
        char p[4096];
        if (i == 0) snprintf(p, sizeof(p), "%s/classes.dex", work);
        else snprintf(p, sizeof(p), "%s/classes%d.dex", work, i + 1);
        FILE *f = fopen(p, "wb");
        if (f) {
            if (dexes[i].data.len) fwrite(dexes[i].data.p, 1, dexes[i].data.len, f);
            fclose(f);
        }
    }

    {
        StrList tmp;
        sl_init(&tmp);
        ep_stub_strings(cls0.p, cls0.len, &tmp);
        for (int i = 0; i < tmp.n; i++) sl_add(&cands, tmp.v[i]);
        sl_clear(&tmp);
    }
    {
        Buf *dbufs = (Buf *) xmalloc(sizeof(Buf) * (size_t) (nd ? nd : 1));
        for (int i = 0; i < nd; i++) dbufs[i] = dexes[i].data;
        EpSupMap sup;
        ep_build_sup(&sup, dbufs, nd);
        ep_entry_factory(&sup, &cands, pkg, &entryCls, &factoryCls);
        ep_sup_free(&sup);
        free(dbufs);
    }

    
    r->keyHex = xstrdup(isExtract ? keyHex : seedStr);
    if (pkg) r->pkg = xstrdup(pkg);
    r->dexCount = nd;
    r->ok = 1;
    okFlag = 1;
    goto done;
    okFlag = 1;
    goto done;

fail:
        r->tail = xstrdup("");

done:
    for (int i = 0; i < nd; i++) { free(dexes[i].data.p); free(dexes[i].name); }
    free(dexes);
    for (int i = 0; i < nblocks; i++) {
        for (int j = 0; j < blocks[i].nrec; j++) free(blocks[i].recs[j].enc.p);
        free(blocks[i].recs);
    }
    free(blocks);
    free(cfg.p);
    sl_clear(&cands);
    free(entryCls);
    free(factoryCls);
    free(pkg);
    free(manifest.p);
    free(cls0.p);
    zip_close(&z);
    (void) haveKey;
    (void) okFlag;

    return 1;
}
