uint32_t ep_uleb(const uint8_t *b, size_t n, size_t *p)
{
    uint32_t r = 0;
    int s = 0;
    while (*p < n) {
        uint8_t x = b[(*p)++];
        r |= (uint32_t) (x & 0x7f) << s;
        s += 7;
        if (x < 0x80) break;
    }
    return r;
}
