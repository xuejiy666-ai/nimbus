void vec_write(Vec *v, const void *p, size_t n)
{
    if (!n) return;
    vec_reserve(v, n);
    memcpy(v->p + v->len, p, n);
    v->len += n;
}

void ep_cleanup(StrList *a, StrList *b, StrList *c)
{
    if (a) sl_clear(a);
    if (b) sl_clear(b);
    if (c) sl_clear(c);
}
