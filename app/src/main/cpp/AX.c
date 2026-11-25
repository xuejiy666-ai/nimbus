void dex_resign(uint8_t *d, size_t n)
{
    if (!d || n < 0x70) return;
    uint8_t sha[20];
    sha1_of(d + 32, n - 32, sha);
    memcpy(d + 12, sha, 20);
    wr32(d, n, 8, adler32_of(d + 12, n - 12));
}
