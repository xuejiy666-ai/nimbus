uint32_t rd32(const uint8_t *b, size_t n, size_t o)
{
    if (!b || o + 4 > n || o > n) return 0;
    return (uint32_t) b[o] | ((uint32_t) b[o + 1] << 8)
         | ((uint32_t) b[o + 2] << 16) | ((uint32_t) b[o + 3] << 24);
}
