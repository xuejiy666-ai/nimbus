void rc4_stream(const uint8_t *key, size_t klen, uint8_t *out, size_t n)
{
    uint8_t *zero = xcalloc(n ? n : 1, 1);
    rc4_crypt(key, klen, zero, out, n);
    free(zero);
}

void *xmalloc(size_t n)
{
    void *p = malloc(n ? n : 1);
    if (!p) oom();
    return p;
}
