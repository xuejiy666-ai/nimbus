uint32_t dp_uleb(const uint8_t *d, size_t n, size_t *off)
{
    uint32_t r = 0;
    int s = 0;
    while (*off < n) {
        uint8_t b = d[(*off)++];
        r |= (uint32_t) (b & 0x7F) << s;
        if (!(b & 0x80)) break;
        s += 7;
        if (s > 28) break;
    }
    return r;
}
