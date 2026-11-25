void vec_reserve(Vec *v, size_t need)
{
    if (v->len + need <= v->cap) return;
    size_t cap = v->cap ? v->cap : 1024;
    while (cap < v->len + need) cap *= 2;
    v->p = xrealloc(v->p, cap);
    v->cap = cap;
}
