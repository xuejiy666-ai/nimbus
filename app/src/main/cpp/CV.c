uint64_t dp_mix64(uint64_t k)
{
    k ^= k >> 33;
    k *= 0xFF51AFD7ED558CCDULL;
    k ^= k >> 33;
    k *= 0xC4CEB9FE1A85EC53ULL;
    k ^= k >> 33;
    return k | 0x0101010101010101ULL;
}

uint32_t bd_ror(uint32_t x, int r) { return (x >> r) | (x << (32 - r)); }
