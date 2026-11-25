int dpt_load(const char *apkPath, DptCtx *c)
{
    memset(c, 0, sizeof(*c));
    if (zip_open(apkPath, &c->z) != 0) return 0;

    ZipEntry *poolEnt = zip_find(&c->z, "assets/OoooooOooo");
    ZipEntry *cfEnt = zip_find(&c->z, "assets/d_shell_data_001");
    if (!poolEnt && !cfEnt) return 0;

    ZipEntry *me = zip_find(&c->z, "AndroidManifest.xml");
    if (me) zip_read(&c->z, me, &c->manifest);
    c->pkg = c->manifest.p ? axml_read_package(c->manifest.p, c->manifest.len) : NULL;
    if (!c->pkg) c->pkg = xstrdup("");

    for (int i = 0; i < c->z.n; i++) {
        const char *nm = c->z.e[i].name;
        if (strncmp(nm, "assets/vwwwwwvwww/", 18) == 0 && ends_with_ci(nm, ".so")) {
            zip_read(&c->z, &c->z.e[i], &c->so);
            if (c->so.p) break;
        }
    }
    if (!c->so.p) {
        for (int i = 0; i < c->z.n; i++) {
            if (!ends_with_ci(c->z.e[i].name, ".so")) continue;
            Buf b = {NULL, 0};
            if (zip_read(&c->z, &c->z.e[i], &b) != 0 || !b.p) { free(b.p); continue; }
            if (dp_memmem(b.p, b.len, "DPT_UNKNOWN_DATA", 16) ||
                dp_memmem(b.p, b.len, "OoooooOooo", 10)) { c->so = b; break; }
            free(b.p);
        }
    }
    if (!c->so.p) return 0;

    dp_ak_candidates(c->so.p, c->so.len, c->ak, &c->nak);
    if (cfEnt) zip_read(&c->z, cfEnt, &c->cf);
    c->verm = c->manifest.p ? dp_axml_version(c->manifest.p, c->manifest.len) : NULL;
    if (c->cf.p && c->nak)
        dp_recover_cfg(c->so.p, c->so.len, c->cf.p, c->cf.len, c->pkg,
                       c->verm ? c->verm : "", c->ak, c->nak,
                       &c->cfg, c->aeskey, &c->klen);
    free(c->verm);
    c->verm = NULL;
    if (!c->cfg) return 2;

    if (poolEnt) zip_read(&c->z, poolEnt, &c->praw);
    if (!c->praw.p ||
        dp_parse_pool(c->praw.p, c->praw.len, &c->ver, &c->pool, &c->npool) != 0)
        return 2;

    c->have_xk = dp_json_int(c->cfg, "insns_xor_key", &c->xk);
    return 1;
}
