void dp_xor_strings(const uint8_t *b, size_t n, const uint8_t k8[8],
                           char out[][33], int *nout, int max)
{
    for (size_t o = 0; o + 2 < n && *nout < max; o++) {
        if (!dp_is_hexc(b[o] ^ k8[0])) continue;
        char tmp[33];
        int m = 0, ok = 0;
        size_t j = o;
        while (j < n && m < 32) {
            int c = b[j] ^ k8[(j - o) & 7];
            if (c == 0) { ok = 1; break; }
            if (c < 0x21 || c > 0x7e) break;
            tmp[m++] = (char) c;
            j++;
        }
        if (ok && m >= 1) {
            tmp[m] = 0;
            int dup = 0;
            for (int t = 0; t < *nout; t++) if (strcmp(out[t], tmp) == 0) dup = 1;
            if (!dup) snprintf(out[(*nout)++], 33, "%s", tmp);
        }
    }
}
