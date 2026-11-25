void wr16(uint8_t *b, size_t n, size_t o, uint32_t v)
{
    if (!b || o + 2 > n) return;
    b[o] = (uint8_t) v;
    b[o + 1] = (uint8_t) (v >> 8);
}

void dp_pool_free(DpPoolDex *a, int n)
{
    if (!a) return;
    for (int i = 0; i < n; i++) free(a[i].v);
    free(a);
}
