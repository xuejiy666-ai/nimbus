int ep_lcg_find(const uint8_t *lib, size_t n, EpLcg **out, int *nout)
{
    size_t wn = n / 4;
    const uint8_t *W = lib;
    typedef struct { size_t idx; uint32_t off; } St;
    St *st = NULL;
    int ns = 0;
    for (size_t i = 0; i < wn; i++) {
        uint32_t w = rd32(W, wn * 4, i * 4);
        if ((w & 0xFFC00000u) == 0xB9000000u) {
            st = (St *) xrealloc(st, sizeof(St) * (size_t) (ns + 1));
            st[ns].idx = i;
            st[ns].off = ((w >> 10) & 0xFFFu) * 4u;
            ns++;
        }
    }
    EpLcg *res = NULL;
    int nr = 0;
    for (int k = 0; k + 2 < ns; k++) {
        size_t a = st[k].idx, b = st[k + 1].idx, c = st[k + 2].idx;
        if (st[k + 1].off != st[k].off + 4 || st[k + 2].off != st[k].off + 8) continue;
        if (!(a < b && b < c && c - a < 40)) continue;
        uint32_t args[3];
        int ok = 1;
        size_t idxs[3] = {a, b, c};
        for (int q = 0; q < 3 && ok; q++) {
            size_t j = idxs[q];
            while (j > 0) {
                uint32_t w = rd32(W, wn * 4, j * 4);
                if ((w & 0xFFFFFC1Fu) == 0xD63F0000u) break;
                j--;
            }
            if (j == 0) { ok = 0; break; }
            uint32_t vals[3] = {0, 0, 0};
            int have[3] = {0, 0, 0};
            size_t from = j > 14 ? j - 14 : 0;
            for (size_t t2 = from; t2 < j; t2++) {
                uint32_t w = rd32(W, wn * 4, t2 * 4);
                uint32_t m = w & 0x7F800000u;
                if (m == 0x52800000u || m == 0x72800000u) {
                    uint32_t rd = w & 0x1Fu;
                    if (rd > 2) continue;
                    uint32_t hw = (w >> 21) & 3u;
                    uint32_t imm = ((w >> 5) & 0xFFFFu) << (16 * hw);
                    vals[rd] = (vals[rd] & ~(0xFFFFu << (16 * hw))) | imm;
                    have[rd] = 1;
                }
            }
            if (!have[0] || !have[1] || !have[2]) { ok = 0; break; }
            args[q] = ep_mix(vals[0], vals[1], vals[2]);
        }
        if (!ok) continue;
        res = (EpLcg *) xrealloc(res, sizeof(EpLcg) * (size_t) (nr + 1));
        res[nr].off = st[k].off;
        res[nr].seed = args[0];
        res[nr].mult = args[1] | 1u;
        res[nr].inc = args[2];
        nr++;
    }
    free(st);
    *out = res;
    *nout = nr;
    return nr > 0 ? 0 : -1;
}
