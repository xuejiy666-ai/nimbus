int bd_printable_ratio(const uint8_t *p, size_t n)
{
    if (!n) return 0;
    size_t c = 0;
    for (size_t i = 0; i < n; i++) if ((p[i] >= 32 && p[i] < 127) || p[i] == 0) c++;
    return (int) (c * 1000 / n);
}
