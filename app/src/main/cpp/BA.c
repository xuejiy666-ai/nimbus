int dex_idx_class(DexIdx *x, const char *desc)
{
    if (!x || !x->hslot || !desc) return -1;
    uint32_t h = hash_str(desc) & (uint32_t) (x->hcap - 1);
    for (int k = 0; k < x->hcap; k++) {
        int slot = (int) ((h + (uint32_t) k) & (uint32_t) (x->hcap - 1));
        int ci = x->hslot[slot];
        if (ci < 0) return -1;
        uint32_t cidx = x->cidx[ci];
        if (cidx < x->nt && x->typ[cidx] && strcmp(x->typ[cidx], desc) == 0) return ci;
    }
    return -1;
}
