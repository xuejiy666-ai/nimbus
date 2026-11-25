void al_add(AttrList *a, const uint8_t *rec)
{
    if (a->n == a->cap) {
        a->cap = a->cap ? a->cap * 2 : 16;
        a->v = xrealloc(a->v, (size_t) a->cap * sizeof(uint8_t *));
    }
    uint8_t *c = xmalloc(20);
    memcpy(c, rec, 20);
    a->v[a->n++] = c;
}
