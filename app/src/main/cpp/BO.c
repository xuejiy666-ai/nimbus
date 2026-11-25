char *utf16le_to_utf8(const uint8_t *p, size_t units)
{
    Vec o;
    vec_init(&o);
    for (size_t i = 0; i < units; i++) {
        uint32_t u = (uint32_t) p[2 * i] | ((uint32_t) p[2 * i + 1] << 8);
        if (u >= 0xD800 && u <= 0xDBFF && i + 1 < units) {
            uint32_t l = (uint32_t) p[2 * i + 2] | ((uint32_t) p[2 * i + 3] << 8);
            if (l >= 0xDC00 && l <= 0xDFFF) {
                uint32_t cp = 0x10000 + ((u - 0xD800) << 10) + (l - 0xDC00);
                put_utf8(&o, cp);
                i++;
                continue;
            }
        }
        put_utf8(&o, u);
    }
    vec_put8(&o, 0);
    char *s = vec_str(&o);
    vec_free(&o);
    return s;
}
