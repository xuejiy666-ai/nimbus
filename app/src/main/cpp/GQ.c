int dp_ay_scan_key(const uint8_t *d, size_t n, uint64_t k,
                          char out[][33], int *nout, int max)
{
    uint8_t kb[8];
    for (int j = 0; j < 8; j++) kb[j] = (uint8_t)((k >> (8 * j)) & 0xff);
    uint8_t f[256];
    for (int b = 0; b < 256; b++) f[b] = (uint8_t) dp_is_hexc(b ^ kb[0]);
    for (size_t i = 0; i + 8 < n && *nout < max; i++) {
        if (!f[d[i]]) continue;
        int ok = 1;
        for (int j = 1; j < 8; j++)
            if (!dp_is_hexc(d[i + j] ^ kb[j])) { ok = 0; break; }
        if (!ok) continue;
        char tmp[33];
        int m = 0, term = 0;
        size_t j = i;
        while (j < n && m <= 32) {
            int c = d[j] ^ kb[(j - i) & 7];
            if (c == 0) { term = 1; break; }
            if (c < 0x20 || c >= 0x7f) break;
            tmp[m++] = (char) c;
            j++;
        }
        if (!term || m < 1 || m > 32) continue;
        tmp[m] = 0;
        int dup = 0;
        for (int t = 0; t < *nout; t++) if (strcmp(out[t], tmp) == 0) { dup = 1; break; }
        if (!dup) snprintf(out[(*nout)++], 33, "%s", tmp);
    }
    return 0;
}
