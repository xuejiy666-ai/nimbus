uint32_t dp_str_off(const uint8_t *d, size_t n, uint32_t si)
{
    uint32_t off = rd32(d, n, 0x3C);
    if ((uint64_t) off + (uint64_t) si * 4 + 4 > n) return 0xFFFFFFFFu;
    uint32_t o = rd32(d, n, (size_t) off + (size_t) si * 4);
    return o < n ? o : 0xFFFFFFFFu;
}
