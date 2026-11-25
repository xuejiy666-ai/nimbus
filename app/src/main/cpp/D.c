uint64_t dp_rd64(const uint8_t *d, size_t n, size_t o)
{
    if (o + 8 > n) return 0;
    return (uint64_t) rd32(d, n, o) | ((uint64_t) rd32(d, n, o + 4) << 32);
}

void *xrealloc(void *p, size_t n)
{
    void *q = realloc(p, n ? n : 1);
    if (!q) oom();
    return q;
}
