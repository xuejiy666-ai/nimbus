int dp_walk_methods(const uint8_t *d, size_t n, DpMethodFn fn, void *ctx)
{
    if (n < 112 || memcmp(d, "dex\n", 4) != 0) return 0;
    uint32_t cds = rd32(d, n, 96), cdo = rd32(d, n, 100);
    if ((uint64_t) cdo + (uint64_t) cds * 32 > n) return 0;
    int total = 0;
    for (uint32_t i = 0; i < cds; i++) {
        uint32_t cdat = rd32(d, n, cdo + i * 32 + 24);
        if (!cdat) continue;
        size_t o = cdat;
        uint32_t sf = dp_uleb(d, n, &o), inf = dp_uleb(d, n, &o);
        uint32_t dm = dp_uleb(d, n, &o), vm = dp_uleb(d, n, &o);
        if (sf + inf > 200000 || dm > 200000 || vm > 200000) continue;
        for (uint32_t k = 0; k < sf + inf; k++) { dp_uleb(d, n, &o); dp_uleb(d, n, &o); }
        for (int pass = 0; pass < 2; pass++) {
            uint32_t cnt = pass ? vm : dm;
            uint32_t idx = 0;
            for (uint32_t k = 0; k < cnt; k++) {
                uint32_t diff = dp_uleb(d, n, &o);
                uint32_t fl = dp_uleb(d, n, &o);
                uint32_t co = dp_uleb(d, n, &o);
                idx += diff;
                if (!co || (uint64_t) co + 16 > n) continue;
                uint32_t sz = rd32(d, n, co + 12);
                if ((uint64_t) co + 16 + (uint64_t) sz * 2 > n) continue;
                total++;
                if (fn && fn(ctx, idx, fl, co, sz)) return total;
            }
        }
    }
    return total;
}
