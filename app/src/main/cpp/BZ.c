void bl_add_own(BufList *l, Buf b)
{
    if (l->n == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 16;
        l->v = xrealloc(l->v, (size_t) l->cap * sizeof(Buf));
    }
    l->v[l->n++] = b;
}
