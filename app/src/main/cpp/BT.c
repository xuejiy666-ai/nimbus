Buf span_at(const uint8_t *b, size_t n, size_t p, int utf8)
{
    size_t start = p;
    if (utf8) {
        if (p >= n) return (Buf) {NULL, 0};
        uint32_t l = b[p++];
        if (l & 0x80) p++;
        if (p >= n) return (Buf) {NULL, 0};
        uint32_t l2 = b[p++];
        if (l2 & 0x80) {
            if (p >= n) return (Buf) {NULL, 0};
            l2 = ((l2 & 0x7f) << 8) | b[p++];
        }
        p += l2;
        p += 1;
    } else {
        uint32_t l = rd16(b, n, p);
        p += 2;
        if (l & 0x8000) {
            l = ((l & 0x7fff) << 16) | rd16(b, n, p);
            p += 2;
        }
        p += 2 * (size_t) l;
        p += 2;
    }
    if (p > n) p = n;
    Buf out;
    out.len = p - start;
    out.p = xmalloc(out.len ? out.len : 1);
    if (out.len) memcpy(out.p, b + start, out.len);
    return out;
}
