char *decode_at(const uint8_t *b, size_t n, size_t p, int utf8)
{
    if (p >= n) return xstrdup("");
    if (utf8) {
        uint32_t l = b[p++];
        if (l & 0x80) p++;
        if (p >= n) return xstrdup("");
        uint32_t l2 = b[p++];
        if (l2 & 0x80) {
            if (p >= n) return xstrdup("");
            l2 = ((l2 & 0x7f) << 8) | b[p++];
        }
        if (p + l2 > n) l2 = (uint32_t) (n - p);
        char *s = xmalloc(l2 + 1);
        memcpy(s, b + p, l2);
        s[l2] = 0;
        return s;
    }
    uint32_t l = rd16(b, n, p);
    p += 2;
    if (l & 0x8000) {
        l = ((l & 0x7fff) << 16) | rd16(b, n, p);
        p += 2;
    }
    if (p + 2 * (size_t) l > n) l = (uint32_t) ((n - p) / 2);
    return utf16le_to_utf8(b + p, l);
}
