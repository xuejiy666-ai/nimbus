void dp_aes_expand(const uint8_t *key, int klen, uint8_t *rk, int *rnr)
{
    int nk = klen / 4, nr = nk + 6, total = 4 * (nr + 1);
    *rnr = nr;
    for (int i = 0; i < total; i++) {
        if (i < nk) {
            rk[4 * i] = key[4 * i]; rk[4 * i + 1] = key[4 * i + 1];
            rk[4 * i + 2] = key[4 * i + 2]; rk[4 * i + 3] = key[4 * i + 3];
        } else {
            uint8_t t[4];
            memcpy(t, rk + 4 * (i - 1), 4);
            if (i % nk == 0) {
                uint8_t u = t[0];
                t[0] = (uint8_t) (g_dsb[t[1]] ^ (uint8_t) (1 << ((i / nk - 1) & 0x1f)));
                t[1] = g_dsb[t[2]]; t[2] = g_dsb[t[3]]; t[3] = g_dsb[u];
            } else if (nk > 6 && i % nk == 4) {
                for (int k = 0; k < 4; k++) t[k] = g_dsb[t[k]];
            }
            for (int k = 0; k < 4; k++) rk[4 * i + k] = (uint8_t) (rk[4 * (i - nk) + k] ^ t[k]);
        }
    }
}
