int dpt_gather(DptCtx *c, const char *outDir)
{
    ZipEntry *iz = zip_find(&c->z, "assets/i11111i111.zip");
    if (!iz) iz = zip_find(&c->z, "assets/i11111i111");
    if (iz) {
        zip_read(&c->z, iz, &c->inner);
        if (c->inner.p) {
            ZipReader zi;
            char tmp[4200];
            snprintf(tmp, sizeof(tmp), "%s/.dp_inner_%d.zip", outDir, (int) getpid());
            FILE *f = fopen(tmp, "wb");
            if (f) { fwrite(c->inner.p, 1, c->inner.len, f); fclose(f); }
            if (zip_open(tmp, &zi) == 0) {
                c->dexes = (Buf *) xmalloc(sizeof(Buf) * (size_t) (zi.n ? zi.n : 1));
                for (int i = 0; i < zi.n; i++) {
                    if (!ends_with_ci(zi.e[i].name, ".dex")) continue;
                    Buf b = {NULL, 0};
                    if (zip_read(&zi, &zi.e[i], &b) != 0) continue;
                    c->dexes[c->nd++] = b;
                }
                zip_close(&zi);
            }
            unlink(tmp);
        }
    }
    if (!c->nd) {
        for (int i = 0; i < c->z.n && !c->nd; i++) {
            const char *nm = c->z.e[i].name;
            if (!(strcmp(nm, "classes.dex") == 0 ||
                  (strncmp(nm, "classes", 7) == 0 && ends_with_ci(nm, ".dex"))))
                continue;
            Buf b = {NULL, 0};
            if (zip_read(&c->z, &c->z.e[i], &b) != 0) continue;
            size_t off = 0;
            int found = 0;
            for (size_t q = 8; q + 4 <= b.len; q++) {
                if (b.p[q] == 'P' && b.p[q + 1] == 'K' && b.p[q + 2] == 3 && b.p[q + 3] == 4) {
                    off = q; found = 1; break;
                }
            }
            if (found) {
                char tmp[4200];
                snprintf(tmp, sizeof(tmp), "%s/.dp_tail_%d.zip", outDir, (int) getpid());
                FILE *f = fopen(tmp, "wb");
                if (f) { fwrite(b.p + off, 1, b.len - off, f); fclose(f); }
                ZipReader zt;
                if (zip_open(tmp, &zt) == 0) {
                    c->dexes = (Buf *) xmalloc(sizeof(Buf) * (size_t) (zt.n ? zt.n : 1));
                    for (int k2 = 0; k2 < zt.n; k2++) {
                        if (!ends_with_ci(zt.e[k2].name, ".dex")) continue;
                        Buf db = {NULL, 0};
                        if (zip_read(&zt, &zt.e[k2], &db) != 0) continue;
                        c->dexes[c->nd++] = db;
                    }
                    zip_close(&zt);
                }
                unlink(tmp);
            }
            free(b.p);
        }
    }
    if (!c->nd) {
        c->dexes = (Buf *) xmalloc(sizeof(Buf) * (size_t) (c->z.n ? c->z.n : 1));
        for (int i = 0; i < c->z.n; i++) {
            const char *nm = c->z.e[i].name;
            if (!(strcmp(nm, "classes.dex") == 0 ||
                  (strncmp(nm, "classes", 7) == 0 && ends_with_ci(nm, ".dex"))))
                continue;
            Buf b = {NULL, 0};
            if (zip_read(&c->z, &c->z.e[i], &b) != 0) continue;
            c->dexes[c->nd++] = b;
        }
    }
    return c->nd ? 1 : 0;
}
