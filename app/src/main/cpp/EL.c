void ep_lcg_dec(uint8_t *d, size_t n, uint32_t seed, uint32_t mult, uint32_t inc)
{
    uint32_t st = seed;
    for (size_t i = 0; i < n; i++) {
        st = st * mult + inc;
        d[i] ^= (uint8_t) (st >> 24);
    }
}
