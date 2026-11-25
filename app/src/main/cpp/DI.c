int bd_cmp_win(const void *a, const void *b)
{
    const BdWin *x = (const BdWin *) a, *y = (const BdWin *) b;
    if (x->cnt < y->cnt) return 1;
    if (x->cnt > y->cnt) return -1;
    return 0;
}
