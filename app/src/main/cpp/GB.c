char *dp_dex_str(const uint8_t *d, size_t n, uint32_t si)
{
    uint32_t p = dp_str_off(d, n, si);
    if (p == 0xFFFFFFFFu) return NULL;
    int sh = 0;
    while (p < n) {
        uint8_t b = d[p++];
        if (!(b & 0x80)) break;
        sh += 7;
        if (sh > 28) return NULL;
    }
    if (p >= n) return NULL;
    size_t q = p;
    while (q < n && d[q]) q++;
    if (q > n) return NULL;
    char *s = (char *) xmalloc(q - p + 1);
    memcpy(s, d + p, q - p);
    s[q - p] = 0;
    return s;
}
