int e2_b64(const char *s, uint8_t **out, size_t *outlen)
{
    static signed char tab[256];
    static int init = 0;
    if (!init) {
        for (int i = 0; i < 256; i++) tab[i] = -1;
        const char *al = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        for (int i = 0; i < 64; i++) tab[(unsigned char) al[i]] = (signed char) i;
        init = 1;
    }
    size_t n = strlen(s), cap = n * 3 / 4 + 4;
    uint8_t *b = (uint8_t *) xmalloc(cap ? cap : 1);
    size_t o = 0;
    unsigned acc = 0;
    int bits = 0;
    for (size_t i = 0; i < n; i++) {
        int c = tab[(unsigned char) s[i]];
        if (c < 0) continue;
        acc = (acc << 6) | (unsigned) c;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            b[o++] = (uint8_t) ((acc >> bits) & 0xff);
        }
    }
    *out = b;
    *outlen = o;
    return 0;
}
