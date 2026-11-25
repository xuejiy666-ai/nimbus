int e2_is_shell_dex(const uint8_t *d, size_t n)
{
    if (n < 112 || memcmp(d, "dex\n", 4) != 0) return 0;
    uint32_t tsz = rd32(d, n, 0x40), toff = rd32(d, n, 0x44);
    uint32_t csz = rd32(d, n, 0x60), coff = rd32(d, n, 0x64);
    static const char *names[] = {"LEpic/Protect;", "LEpic/ProtectApp;", "LEpic/SoLoader;", NULL};
    if (coff >= n || (uint64_t) coff + (uint64_t) csz * 32 > n) return 0;
    for (uint32_t i = 0; i < csz; i++) {
        uint32_t ti = rd32(d, n, (size_t) coff + (size_t) i * 32);
        if (ti >= tsz) continue;
        if ((uint64_t) toff + (uint64_t) ti * 4 + 4 > n) continue;
        uint32_t so = rd32(d, n, (size_t) toff + (size_t) ti * 4);
        if (so >= n) continue;
        size_t p = so;
        uint32_t v = 0;
        int sh = 0;
        while (p < n) {
            uint8_t b = d[p++];
            v |= (uint32_t)(b & 0x7f) << sh;
            if (!(b & 0x80)) break;
            sh += 7;
            if (sh > 28) break;
        }
        (void) v;
        for (int k = 0; names[k]; k++) {
            size_t L = strlen(names[k]);
            if (p + L <= n && memcmp(d + p, names[k], L) == 0 &&
                (p + L == n || d[p + L] == 0)) return 1;
        }
    }
    return 0;
}
