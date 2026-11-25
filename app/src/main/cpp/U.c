uint16_t rd16(const uint8_t *b, size_t n, size_t o)
{
    if (!b || o + 2 > n) return 0;
    return (uint16_t) (b[o] | (b[o + 1] << 8));
}

void bl_free(BufList *l)
{
    for (int i = 0; i < l->n; i++) free(l->v[i].p);
    free(l->v);
    l->v = NULL; l->n = l->cap = 0;
}
