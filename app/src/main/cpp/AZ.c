void idx_hash_build(DexIdx *x)
{
    int cap = 64;
    while (cap < (int) x->nd * 2 + 8) cap <<= 1;
    x->hcap = cap;
    x->hslot = xmalloc((size_t) cap * sizeof(int));
    for (int i = 0; i < cap; i++) x->hslot[i] = -1;
    for (uint32_t i = 0; i < x->nd; i++) {
        uint32_t ci = x->cidx[i];
        if (ci >= x->nt) continue;
        const char *nm = x->typ[ci];
        if (!nm || !nm[0]) continue;
        uint32_t h = hash_str(nm) & (uint32_t) (cap - 1);
        for (int k = 0; k < cap; k++) {
            int slot = (int) ((h + (uint32_t) k) & (uint32_t) (cap - 1));
            if (x->hslot[slot] < 0) { x->hslot[slot] = (int) i; break; }
        }
    }
}
