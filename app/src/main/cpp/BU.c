Buf encode_string(const char *s, int utf8)
{
    Vec o;
    vec_init(&o);
    size_t units = 0;
    uint32_t *u = utf8_to_utf16(s, &units);
    if (utf8) {
        size_t blen = strlen(s);
        vec_uleb(&o, (uint32_t) units);
        vec_uleb(&o, (uint32_t) blen);
        vec_write(&o, s, blen);
        vec_put8(&o, 0);
    } else {
        vec_put16(&o, (uint32_t) units);
        for (size_t i = 0; i < units; i++) vec_put16(&o, u[i]);
        vec_put8(&o, 0);
        vec_put8(&o, 0);
    }
    free(u);
    Buf b;
    b.p = o.p;
    b.len = o.len;
    return b;
}
