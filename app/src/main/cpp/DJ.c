int bd_cmp_win2(const void *a, const void *b)
{
    const BdWin2 *x = (const BdWin2 *) a, *y = (const BdWin2 *) b;
    if (x->cnt < y->cnt) return 1;
    if (x->cnt > y->cnt) return -1;
    return 0;
}
