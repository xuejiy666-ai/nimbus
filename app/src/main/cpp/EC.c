int ep_dex_methods(const uint8_t *d, size_t n, EpMethodMap *m)
{
    if (n < 0x70 || memcmp(d, "dex\n", 4) != 0) return -1;
    uint32_t ms = rd32(d, n, 88);
    uint32_t cs = rd32(d, n, 96), co = rd32(d, n, 100);
    ep_mm_init(m, ms ? ms : 1);
    for (uint32_t ci = 0; ci < cs; ci++) {
        size_t o = (size_t) co + 32u * ci;
        if (o + 32 > n) { free(m->t); return -1; }
        uint32_t cdo = rd32(d, n, o + 24);
        if (!cdo) continue;
        size_t p = cdo;
        uint32_t sf = ep_uleb(d, n, &p);
        uint32_t inf = ep_uleb(d, n, &p);
        uint32_t dm = ep_uleb(d, n, &p);
        uint32_t vm = ep_uleb(d, n, &p);
        if (sf + inf > 200000u || dm + vm > 200000u) { free(m->t); return -1; }
        for (uint32_t i = 0; i < sf + inf; i++) {
            ep_uleb(d, n, &p);
            ep_uleb(d, n, &p);
            if (p > n) { free(m->t); return -1; }
        }
        for (int k = 0; k < 2; k++) {
            uint32_t mc = k ? vm : dm;
            uint32_t mid = 0;
            for (uint32_t i = 0; i < mc; i++) {
                uint32_t df = ep_uleb(d, n, &p);
                mid += df;
                ep_uleb(d, n, &p);
                uint32_t coff = ep_uleb(d, n, &p);
                if (p > n) { free(m->t); return -1; }
                if (mid >= ms) { free(m->t); return -1; }
                ep_mm_put(m, mid, coff);
            }
        }
    }
    return 0;
}
