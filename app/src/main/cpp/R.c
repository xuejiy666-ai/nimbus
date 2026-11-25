uint64_t bd_rd64(const uint8_t *p)
{
    uint64_t v = 0;
    for (int i = 0; i < 8; i++) v |= ((uint64_t) p[i]) << (8 * i);
    return v;
}

void sl_clear(StrList *s)
{
    for (int i = 0; i < s->n; i++) free(s->v[i]);
    free(s->v);
    s->v = NULL; s->n = s->cap = 0;
}
