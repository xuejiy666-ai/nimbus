void ss_grow(SSet *s)
{
    int ncap = s->cap ? s->cap * 2 : 1024;
    char **nv = xcalloc((size_t) ncap, sizeof(char *));
    for (int i = 0; i < s->cap; i++) {
        if (!s->v[i]) continue;
        uint32_t h = shash(s->v[i], ncap);
        while (nv[h]) h = (h + 1) & (uint32_t) (ncap - 1);
        nv[h] = s->v[i];
    }
    free(s->v);
    s->v = nv;
    s->cap = ncap;
}
