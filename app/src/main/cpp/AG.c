uint32_t adler32_of(const uint8_t *b, size_t n)
{
    uint32_t a = 1, s = 0;
    for (size_t i = 0; i < n; i++) {
        a += b[i];
        if (a >= 65521) a -= 65521;
        s += a;
        if (s >= 65521) s -= 65521;
    }
    return (s << 16) | a;
}
