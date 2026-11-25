int container_trailer(const uint8_t *d, size_t n)
{
    if (!d || n < 0x28 || !has_at(d, n, 0, "dex\n")) return 0;
    uint32_t fs = rd32(d, n, 0x20);
    if (fs >= 8 && (size_t) fs <= n && has_at(d, n, fs - 4, "fdex")) return (int) fs;
    if (n >= 8 && has_at(d, n, n - 4, "fdex")) return (int) n;
    return 0;
}
