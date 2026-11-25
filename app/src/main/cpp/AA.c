char *hex_of(const uint8_t *b, size_t n)
{
    static const char *D = "0123456789abcdef";
    char *s = xmalloc(n * 2 + 1);
    for (size_t i = 0; i < n; i++) {
        s[2 * i] = D[b[i] >> 4];
        s[2 * i + 1] = D[b[i] & 0x0f];
    }
    s[2 * n] = 0;
    return s;
}
