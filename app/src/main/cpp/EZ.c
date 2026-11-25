int ep_cmp_dex(const void *a, const void *b)
{
    const EpDex *x = (const EpDex *) a, *y = (const EpDex *) b;
    return ep_dex_ord(x->name) - ep_dex_ord(y->name);
}

int dp_return_op(int op)
{
    return op == 0x0E || op == 0x0F || op == 0x10 || op == 0x11 || op == 0x12;
}
