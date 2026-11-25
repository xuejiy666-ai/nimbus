int e2_cmp_size(const void *a, const void *b)
{
    const Buf *x = (const Buf *) a, *y = (const Buf *) b;
    if (x->len < y->len) return 1;
    if (x->len > y->len) return -1;
    return 0;
}

void sl_init(StrList *s)
{
    s->v = NULL; s->n = 0; s->cap = 0;
}
