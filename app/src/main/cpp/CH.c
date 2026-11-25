int ep_cmp_bufsize(const void *a, const void *b)
{
    const Buf *x = (const Buf *) a, *y = (const Buf *) b;
    if (x->len < y->len) return 1;
    if (x->len > y->len) return -1;
    return 0;
}

void ss_init(SSet *s) { s->v = NULL; s->cap = 0; s->n = 0; }
