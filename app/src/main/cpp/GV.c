int dp_recover_cfg(const uint8_t *so, size_t son, const uint8_t *cf, size_t cn,
                          const char *pkg, const char *verm, uint8_t aks[8][16], int nak,
                          char **out, uint8_t key[32], int *klen)
{
    char mat[200];
    for (int i = 0; i < nak; i++) {
        if (dp_try_cfg(aks[i], cf, cn, NULL, out, key, klen)) return 1;
        snprintf(mat, sizeof(mat), "%s", pkg);
        if (dp_try_cfg(aks[i], cf, cn, mat, out, key, klen)) return 1;
        if (verm && *verm) {
            snprintf(mat, sizeof(mat), "%s_%s", pkg, verm);
            if (dp_try_cfg(aks[i], cf, cn, mat, out, key, klen)) return 1;
        }
        snprintf(mat, sizeof(mat), "%s_unknown", pkg);
        if (dp_try_cfg(aks[i], cf, cn, mat, out, key, klen)) return 1;
        
        for (int a = 0; a <= 22; a++) {
            for (int b = 0; b < 4; b++) {
                snprintf(mat, sizeof(mat), "%s_2.%d.%d", pkg, a, b);
                if (dp_try_cfg(aks[i], cf, cn, mat, out, key, klen)) return 1;
            }
        }
    }
    
    char (*cand)[33] = (char (*)[33]) xmalloc(33 * 600);
    int nc = dp_ay_anchor_keys(so, son, cand, 600);
    for (int i = 0; i < nak; i++)
        for (int k = 0; k < nc; k++) {
            snprintf(mat, sizeof(mat), "%s_%s", pkg, cand[k]);
            if (dp_try_cfg(aks[i], cf, cn, mat, out, key, klen)) { free(cand); return 1; }
        }
    nc = dp_ay_build_keys(so, son, cand, 600);
    for (int i = 0; i < nak; i++) {
        for (int k = 0; k < nc; k++) {
            snprintf(mat, sizeof(mat), "%s_%s", pkg, cand[k]);
            if (dp_try_cfg(aks[i], cf, cn, mat, out, key, klen)) { free(cand); return 1; }
        }
    }
    free(cand);
    return 0;
}
