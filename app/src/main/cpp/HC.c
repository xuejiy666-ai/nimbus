int dp_collect_codes(const uint8_t *d, size_t n, DpCode **out)
{
    if (n < 112 || memcmp(d, "dex\n", 4) != 0) return 0;
    uint32_t cds = rd32(d, n, 96), cdo = rd32(d, n, 100);
    if (cdo + (uint64_t) cds * 32 > n) return 0;
    DpCode *arr = NULL;
    int na = 0;
    for (uint32_t i = 0; i < cds; i++) {
        uint32_t cdat = rd32(d, n, cdo + i * 32 + 24);
        if (!cdat) continue;
        size_t o = cdat;
        uint32_t sf = dp_uleb(d, n, &o), inf = dp_uleb(d, n, &o);
        uint32_t dm = dp_uleb(d, n, &o), vm = dp_uleb(d, n, &o);
        if (sf + inf > 200000 || dm + vm > 200000) break;
        for (uint32_t k = 0; k < sf + inf; k++) { dp_uleb(d, n, &o); dp_uleb(d, n, &o); }
        for (int pass = 0; pass < 2; pass++) {
            uint32_t cnt = pass ? vm : dm;
            uint32_t idx = 0;
            for (uint32_t k = 0; k < cnt; k++) {
                uint32_t diff = dp_uleb(d, n, &o);
                dp_uleb(d, n, &o);
                uint32_t co = dp_uleb(d, n, &o);
                idx += diff;
                if (!co || co + 16 > n) continue;
                arr = (DpCode *) xrealloc(arr, sizeof(DpCode) * (size_t) (na + 1));
                arr[na].mi = idx;
                arr[na].bytes = rd32(d, n, co + 12) * 2;
                na++;
            }
        }
    }
    *out = arr;
    return na;
}
