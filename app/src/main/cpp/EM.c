char *ep_type_name(const uint8_t *d, size_t n, uint32_t ti)
{
    uint32_t so = rd32(d, n, 0x3C), to = rd32(d, n, 0x44);
    if ((size_t) to + 4u * ti + 4 > n) return NULL;
    uint32_t si = rd32(d, n, (size_t) to + 4u * ti);
    if ((size_t) so + 4u * si + 4 > n) return NULL;
    size_t p = rd32(d, n, (size_t) so + 4u * si);
    if (p >= n) return NULL;
    ep_uleb(d, n, &p);
    size_t e = p;
    while (e < n && d[e]) e++;
    char *s = (char *) xmalloc(e - p + 1);
    memcpy(s, d + p, e - p);
    s[e - p] = 0;
    return s;
}
