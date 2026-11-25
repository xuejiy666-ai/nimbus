void wr32(uint8_t *b, size_t n, size_t o, uint32_t v)
{
    if (!b || o + 4 > n) return;
    b[o] = (uint8_t) v;
    b[o + 1] = (uint8_t) (v >> 8);
    b[o + 2] = (uint8_t) (v >> 16);
    b[o + 3] = (uint8_t) (v >> 24);
}
