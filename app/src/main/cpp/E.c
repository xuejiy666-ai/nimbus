char *xstrdup(const char *s)
{
    size_t n = s ? strlen(s) : 0;
    char *p = xmalloc(n + 1);
    if (n) memcpy(p, s, n);
    p[n] = 0;
    return p;
}

void vec_put16(Vec *v, uint32_t x)
{
    uint8_t t[2] = {(uint8_t) x, (uint8_t) (x >> 8)};
    vec_write(v, t, 2);
}
