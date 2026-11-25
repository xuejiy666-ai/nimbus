int ep_cmp_ent(const void *a, const void *b)
{
    const EpEnt *x = (const EpEnt *) a, *y = (const EpEnt *) b;
    if (x->size < y->size) return -1;
    if (x->size > y->size) return 1;
    return 0;
}
