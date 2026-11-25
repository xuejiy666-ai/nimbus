int dp_method_ref(const uint8_t *d, size_t n, uint32_t mi, char **cls, char **nm)
{
    uint32_t off = rd32(d, n, 0x5C);
    if ((uint64_t) off + (uint64_t) mi * 8 + 8 > n) return 0;
    uint32_t e = off + mi * 8;
    uint32_t ci = rd16(d, n, e);
    uint32_t ni = rd32(d, n, e + 4);
    *cls = dp_type_name(d, n, ci);
    *nm = dp_dex_str(d, n, ni);
    return 1;
}
