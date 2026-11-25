uint32_t uleb_read(const uint8_t *b, size_t n, size_t *pos)
{
    uint32_t r = 0;
    int s = 0;
    size_t p = *pos;
    for (;;) {
        if (p >= n) break;
        uint8_t x = b[p++];
        r |= (uint32_t) (x & 0x7f) << s;
        s += 7;
        if (!(x & 0x80)) break;
        if (s > 28) break;
    }
    *pos = p;
    return r;
}
