int dp_ak_candidates(const uint8_t *d, size_t n, uint8_t out[8][16], int *nout)
{
    int k = 0;
    uint8_t t[16];
    if (dp_elf_sym(d, n, "DPT_UNKNOWN_DATA", t)) { memcpy(out[k++], t, 16); }
    int found = 0;
    for (size_t i = 0; i + 16 < n && found < 4; i++) {
        if (d[i + 3] == 0x20 && d[i + 9] == 0x74) {
            int dup = 0;
            for (int j = 0; j < k; j++) if (memcmp(out[j], d + i, 16) == 0) dup = 1;
            if (!dup) { memcpy(out[k++], d + i, 16); found++; }
        }
    }
    *nout = k;
    return k;
}
