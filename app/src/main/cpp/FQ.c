uint64_t e2_rd64(const uint8_t *d, size_t n, size_t o)
{
    if (o + 8 > n) return 0;
    uint64_t v = 0;
    for (int i = 7; i >= 0; i--) v = (v << 8) | d[o + (size_t) i];
    return v;
}

uint64_t dp_ay_gen_key(int line)
{
    return dp_mix64((uint64_t) line);
}
