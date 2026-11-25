int dex_valid(const uint8_t *d, size_t off, size_t len)
{
    if (!d || len < 0x70) return 0;
    if (!has_at(d, off + len, off, "dex\n")) return 0;
    if (rd32(d, off + len, off + 0x20) != len) return 0;
    if (rd32(d, off + len, off + 8) != adler32_of(d + off + 12, len - 12)) return 0;
    uint8_t sha[20];
    sha1_of(d + off + 32, len - 32, sha);
    return memcmp(sha, d + off + 12, 20) == 0;
}
