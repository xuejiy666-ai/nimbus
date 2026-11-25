int container_blocks(const uint8_t *d, size_t n, CBlock **out, int *count)
{
    *out = NULL;
    *count = 0;
    int fs = container_trailer(d, n);
    if (!fs) return -1;
    uint32_t cont = rd32(d, n, (size_t) fs - 8);
    if (cont == 0 || (size_t) cont >= n) return -2;
    uint32_t cnt = rd32(d, n, cont);
    size_t blk = (size_t) cont + 4;
    int cap = 8, num = 0;
    CBlock *arr = xcalloc((size_t) cap, sizeof(CBlock));

    for (uint32_t i = 0; i < cnt && i < 256; i++) {
        uint32_t blen = rd32(d, n, blk);
        uint32_t nlen = rd32(d, n, blk + 4);
        size_t ln = 0, dn = 0;
        size_t noff = cut_from(n, blk + 8, nlen, &ln);
        char *nm = xmalloc(ln + 1);
        if (ln) memcpy(nm, d + noff, ln);
        nm[ln] = 0;
        uint32_t dlen = rd32(d, n, blk + 8 + nlen);
        size_t doff = cut_from(n, blk + 12 + nlen, dlen, &dn);
        Buf data = {NULL, 0};
        if (dn) {
            data.p = xmalloc(dn);
            memcpy(data.p, d + doff, dn);
            data.len = dn;
        }
        if (num == cap) {
            cap *= 2;
            arr = xrealloc(arr, (size_t) cap * sizeof(CBlock));
        }
        arr[num].name = nm;
        arr[num].data = data;
        num++;
        if (blen == 0) break;
        blk += blen;
    }
    if (num == 0) { free(arr); return -3; }
    *out = arr;
    *count = num;
    return 0;
}
