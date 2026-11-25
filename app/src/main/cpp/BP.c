uint32_t *utf8_to_utf16(const char *s, size_t *units)
{
    size_t n = strlen(s);
    uint32_t *out = xmalloc((n + 1) * sizeof(uint32_t));
    size_t k = 0;
    for (size_t i = 0; i < n;) {
        uint8_t c = (uint8_t) s[i];
        uint32_t cp;
        if (c < 0x80) { cp = c; i += 1; }
        else if ((c & 0xE0) == 0xC0 && i + 1 < n) { cp = (uint32_t) (c & 0x1F) << 6 | (s[i + 1] & 0x3F); i += 2; }
        else if ((c & 0xF0) == 0xE0 && i + 2 < n) {
            cp = (uint32_t) (c & 0x0F) << 12 | ((uint32_t) (s[i + 1] & 0x3F) << 6) | (s[i + 2] & 0x3F);
            i += 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < n) {
            cp = (uint32_t) (c & 0x07) << 18 | ((uint32_t) (s[i + 1] & 0x3F) << 12)
               | ((uint32_t) (s[i + 2] & 0x3F) << 6) | (s[i + 3] & 0x3F);
            i += 4;
        } else { cp = 0xFFFD; i += 1; }
        if (cp >= 0x10000) {
            cp -= 0x10000;
            out[k++] = 0xD800 + (cp >> 10);
            out[k++] = 0xDC00 + (cp & 0x3FF);
        } else {
            out[k++] = cp;
        }
    }
    *units = k;
    return out;
}
