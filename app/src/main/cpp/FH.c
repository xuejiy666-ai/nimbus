char *e2_unescape(const char *s, const char *end)
{
    Vec v;
    vec_init(&v);
    while (s < end) {
        if (*s != '\\') { vec_put8(&v, *s++); continue; }
        s++;
        if (s >= end) break;
        switch (*s) {
        case 'n': vec_put8(&v, '\n'); s++; break;
        case 'r': vec_put8(&v, '\r'); s++; break;
        case 't': vec_put8(&v, '\t'); s++; break;
        case 'u': {
            unsigned v2 = 0;
            s++;
            for (int i = 0; i < 4 && s < end; i++, s++) {
                char ch = *s;
                unsigned d = (ch >= '0' && ch <= '9') ? (unsigned)(ch - '0')
                           : (ch >= 'a' && ch <= 'f') ? (unsigned)(ch - 'a' + 10)
                           : (ch >= 'A' && ch <= 'F') ? (unsigned)(ch - 'A' + 10) : 0;
                v2 = (v2 << 4) | d;
            }
            if (v2 < 0x80) vec_put8(&v, (int) v2);
            else if (v2 < 0x800) {
                vec_put8(&v, 0xC0 | (v2 >> 6));
                vec_put8(&v, 0x80 | (v2 & 0x3F));
            } else {
                vec_put8(&v, 0xE0 | (v2 >> 12));
                vec_put8(&v, 0x80 | ((v2 >> 6) & 0x3F));
                vec_put8(&v, 0x80 | (v2 & 0x3F));
            }
            break;
        }
        default: vec_put8(&v, *s++); break;
        }
    }
    vec_put8(&v, 0);
    return (char *) v.p;
}
