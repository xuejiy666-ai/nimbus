int dp_ay_anchor_keys(const uint8_t *d, size_t n, char out[][33], int max)
{
    int nout = 0;
    static const uint8_t anc[16] = {0xD9,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                    0xE7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    size_t i = 0;
    while (i + 24 <= n && nout < max) {
        const uint8_t *hit = (const uint8_t *) memchr(d + i, 0xD9, n - i - 23);
        if (!hit) break;
        i = (size_t) (hit - d);
        if (memcmp(d + i, anc, 16) == 0) {
            uint8_t k8[8];
            memcpy(k8, d + i + 16, 8);
            dp_xor_strings(d, n, k8, out, &nout, max);
        }
        i++;
    }
    return nout;
}
