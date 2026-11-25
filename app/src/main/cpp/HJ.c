int sc_unpack_run(const char *apkPath, const char *outDir, UnpackResult *r)
{
    memset(r, 0, sizeof(*r));
    FILE *probe = fopen(apkPath, "rb");
    if (!probe) return 0;
    fclose(probe);
    mkdir_p(outDir);

    if (epic_try(apkPath, outDir, r)) return r->ok;
    if (epic2_try(apkPath, outDir, r)) return r->ok;
    if (dpt_try(apkPath, outDir, r)) return r->ok;
    if (baidu_try(apkPath, outDir, r)) return r->ok;

    ZipReader z;
    memset(&z, 0, sizeof(z));
    Buf manifest = {NULL, 0};
    Buf container = {NULL, 0};
    CBlock *blocks = NULL;
    int nb = 0;
    Hit *hits = NULL;
    int nh = 0;
    DexJob *jobs = NULL;
    char pkgDir[4096];
    int rc = 0;

    if (zip_open(apkPath, &z) != 0) return 0;
    ZipEntry *me = zip_find(&z, "AndroidManifest.xml");
    if (me) zip_read(&z, me, &manifest);

    for (int i = 0; i < z.n; i++) {
        ZipEntry *e = &z.e[i];
        if (!ends_with_ci(e->name, ".dex")) continue;
        Buf b = {NULL, 0};
        if (zip_read(&z, e, &b) != 0) continue;
        if (!container.p && container_trailer(b.p, b.len) > 0) container = b;
        else free(b.p);
    }
    if (!container.p) goto done;
    if (container_blocks(container.p, container.len, &blocks, &nb) != 0) goto done;
    free(container.p);
    container.p = NULL;

    r->pkg = manifest.p ? axml_read_package(manifest.p, manifest.len) : NULL;
    if (!r->pkg || !r->pkg[0]) goto done;

    snprintf(pkgDir, sizeof(pkgDir), "%s/%s", outDir, r->pkg);
    if (mkdir_p(pkgDir) == 0) outDir = pkgDir;
    r->outDir = xstrdup(outDir);

    uint8_t key[16];
    package_key(r->pkg, key);
    r->keyHex = hex_of(key, 16);
    block_scan(blocks, nb, key, 16, 1, &hits, &nh);
    if (!nh) goto done;

    jobs = (DexJob *) xcalloc((size_t) nh, sizeof(DexJob));
    for (int i = 0; i < nh; i++) {
        char path[4096];
        if (i == 0) snprintf(path, sizeof(path), "%s/classes.dex", outDir);
        else snprintf(path, sizeof(path), "%s/classes%d.dex", outDir, i + 1);
        jobs[i].src = hits[i].dex.p;
        jobs[i].slen = hits[i].dex.len;
        jobs[i].pkg = r->pkg;
        jobs[i].path = xstrdup(path);
    }
    {
        DexPool pool;
        pool.jobs = jobs;
        pool.n = nh;
        pool.next = 0;
        pthread_mutex_init(&pool.mu, NULL);
        int nt = nh < 4 ? nh : 4;
        pthread_t th[4];
        int started = 0;
        for (int t = 0; t < nt; t++)
            if (pthread_create(&th[t], NULL, dex_pool_run, &pool) == 0) started++;
        if (!started) {
            for (int i = 0; i < nh; i++) dex_job_run(&jobs[i]);
        } else {
            for (int t = 0; t < started; t++) pthread_join(th[t], NULL);
        }
        pthread_mutex_destroy(&pool.mu);
    }
    for (int i = 0; i < nh; i++) {
        if (jobs[i].ok) r->dexCount++;
        free((void *) jobs[i].path);
        free(jobs[i].plain.p);
        free(jobs[i].z.p);
    }
    free(jobs);
    jobs = NULL;
    r->ok = r->dexCount > 0;

done:
    if (hits) hits_free(hits, nh);
    if (blocks) container_blocks_free(blocks, nb);
    free(container.p);
    free(manifest.p);
    zip_close(&z);
    rc = 1;
    return rc;
}
