int bd_match(const uint8_t *so, size_t son, BdHexList *lx, StrList *mds, ZipReader *z,
                    char alg[17], char mode[17])
{
    for (int mi = 0; mi < mds->n; mi++) {
        ZipEntry *e = zip_find(z, mds->v[mi]);
        if (!e) continue;
        Buf md = {NULL, 0};
        if (zip_read(z, e, &md) != 0) continue;
        for (int i = 0; i < lx->n; i++) {
            for (int d = -1; d <= 1; d += 2) {
                int j = i + d;
                if (j < 0 || j >= lx->n) continue;
                long diff = (long) lx->v[j].pos - (long) lx->v[i].pos;
                if (diff < 0) diff = -diff;
                if (diff > 64) continue;
                const char *pair[2] = {lx->v[i].s, lx->v[j].s};
                for (int t = 0; t < 2; t++) {
                    uint8_t key[16], nonce[12], blk[16];
                    size_t take = md.len < 16 ? md.len : 16;
                    if (take < 4) break;
                    memcpy(blk, md.p, take);
                    bd_kdf((const uint8_t *) pair[t], 16,
                           (const uint8_t *) mds->v[mi], strlen(mds->v[mi]), pair[1 - t], key, nonce);
                    bd_ctr(blk, take, key, nonce, take);
                    if (memcmp(blk, "md01", 4) == 0) {
                        snprintf(alg, 17, "%s", pair[t]);
                        snprintf(mode, 17, "%s", pair[1 - t]);
                        free(md.p);
                        (void) so; (void) son;
                        return 1;
                    }
                }
            }
        }
        free(md.p);
    }
    return 0;
}
