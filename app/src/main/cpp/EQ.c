void ep_build_sup(EpSupMap *m, Buf *dexes, int nd)
{
    ep_sup_init(m, 4096);
    for (int i = 0; i < nd; i++) {
        const uint8_t *d = dexes[i].p;
        size_t n = dexes[i].len;
        if (n < 0x70 || memcmp(d, "dex\n", 4) != 0) continue;
        uint32_t cs = rd32(d, n, 0x60), co = rd32(d, n, 0x64);
        for (uint32_t ci = 0; ci < cs; ci++) {
            size_t o = (size_t) co + 32u * ci;
            if (o + 32 > n) break;
            uint32_t nt = rd32(d, n, o), st = rd32(d, n, o + 8);
            char *nm = ep_type_name(d, n, nt);
            if (!nm) continue;
            char *sp = st ? ep_type_name(d, n, st) : NULL;
            ep_sup_put(m, nm, sp ? sp : "");
            free(nm);
            free(sp);
        }
    }
}
