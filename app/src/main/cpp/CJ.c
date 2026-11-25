void ss_add(SSet *s, const char *x)
{
    if (!x || !x[0]) return;
    if (!s->cap || s->n * 4 >= s->cap * 3) ss_grow(s);
    uint32_t h = shash(x, s->cap);
    while (s->v[h]) {
        if (strcmp(s->v[h], x) == 0) return;
        h = (h + 1) & (uint32_t) (s->cap - 1);
    }
    s->v[h] = xstrdup(x);
    s->n++;
}
