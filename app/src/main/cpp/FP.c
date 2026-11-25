Buf e2_dec_buf(const uint8_t *d, size_t n, int method, const char *xork, const char *rc4k)
{
    Buf o = {NULL, 0};
    o.p = (uint8_t *) xmalloc(n ? n : 1);
    o.len = n;
    if (method == 0) {
        int k = e2_xor_key(xork);
        for (size_t i = 0; i < n; i++) o.p[i] = d[i] ^ (uint8_t) k;
    } else {
        if (!rc4k || !*rc4k) { free(o.p); o.p = NULL; o.len = 0; return o; }
        rc4_crypt((const uint8_t *) rc4k, strlen(rc4k), d, o.p, n);
    }
    return o;
}
