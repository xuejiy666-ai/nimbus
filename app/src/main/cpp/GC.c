char *dp_type_name(const uint8_t *d, size_t n, uint32_t ti)
{
    uint32_t off = rd32(d, n, 0x44);
    if ((uint64_t) off + (uint64_t) ti * 4 + 4 > n) return NULL;
    return dp_dex_str(d, n, rd32(d, n, (size_t) off + (size_t) ti * 4));
}
