int ep_dex_ok(const uint8_t *d, size_t n)
{
    if (n < 0x70 || memcmp(d, "dex\n", 4) != 0) return 0;
    if (rd32(d, n, 0x20) != (uint32_t) n) return 0;
    return (uint32_t) adler32(1L, d + 12, (uInt) (n - 12)) == rd32(d, n, 8);
}
