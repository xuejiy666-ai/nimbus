void sl_add_own(StrList *s, char *x)
{
    if (s->n == s->cap) {
        int cap = s->cap ? s->cap * 2 : 16;
        s->v = xrealloc(s->v, (size_t) cap * sizeof(char *));
        s->cap = cap;
    }
    s->v[s->n++] = x;
}
