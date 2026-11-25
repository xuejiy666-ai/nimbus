int dp_field_ref(const uint8_t *d, size_t n, uint32_t fi, char **cls)
{
    uint32_t off = rd32(d, n, 0x54);
    if ((uint64_t) off + (uint64_t) fi * 8 + 8 > n) return 0;
    uint32_t e = off + fi * 8;
    *cls = dp_type_name(d, n, rd16(d, n, e));
    return 1;
}
