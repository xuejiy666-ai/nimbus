int bd_cmp_cand(const void *a, const void *b)
{
    const BdCand *x = (const BdCand *) a, *y = (const BdCand *) b;
    if (x->score < y->score) return 1;
    if (x->score > y->score) return -1;
    return 0;
}
