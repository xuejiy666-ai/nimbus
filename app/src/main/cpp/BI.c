void dex_collect_classes(const uint8_t *d, size_t n, StrList *out)
{
    DexIdx x;
    if (dex_idx_open(&x, d, n) != 0) return;
    for (uint32_t i = 0; i < x.nd; i++) {
        uint32_t ci = x.cidx[i];
        if (ci < x.nt && x.typ[ci] && x.typ[ci][0])
            sl_add(out, x.typ[ci]);          
    }
    dex_idx_free(&x);
}
