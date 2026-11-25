int dex_drop_classes(uint8_t *b, size_t n, const StrList *prefixes, const StrList *also)
{
    uint32_t cnt = rd32(b, n, 0x60);
    uint32_t off = rd32(b, n, 0x64);
    if (cnt == 0 || cnt > 1000000 || off == 0 || (size_t) off + 32u * cnt > n) return 0;

    DexIdx x;
    if (dex_idx_open(&x, b, n) != 0) return 0;

    int cap = (int) cnt;
    uint8_t *keep = xmalloc((size_t) cap * 32);
    int kn = 0;
    for (uint32_t i = 0; i < cnt; i++) {
        const char *name = (x.cidx[i] < x.nt) ? x.typ[x.cidx[i]] : "";
        if (prefix_match(prefixes, name) && !in_keep(name)) {
            continue;
        }
        if (also && sl_contains(also, name)) continue;
        memcpy(keep + (size_t) kn * 32, b + off + 32u * i, 32);
        kn++;
    }
    int dropped = (int) cnt - kn;
    if (dropped == 0) { free(keep); dex_idx_free(&x); return 0; }
    memcpy(b + off, keep, (size_t) kn * 32);
    memset(b + off + (size_t) kn * 32, 0, (size_t) dropped * 32);
    free(keep);
    wr32(b, n, 0x60, (uint32_t) kn);

    uint32_t mo = rd32(b, n, 0x34);
    if (mo > 0 && (size_t) mo + 4 <= n) {
        uint32_t items = rd32(b, n, mo);
        for (uint32_t k = 0; k < items; k++) {
            if ((size_t) mo + 8 + 12u * k + 4 > n) break;
            if (rd16(b, n, mo + 4 + 12u * k) == 0x0006)
                wr32(b, n, mo + 8 + 12u * k, (uint32_t) kn);
        }
    }
    dex_idx_free(&x);
    return dropped;
}
