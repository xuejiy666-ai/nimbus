void ep_stub_strings(const uint8_t *d, size_t n, StrList *out)
{
    if (n < 0x70 || memcmp(d, "dex\n", 4) != 0) return;
    uint32_t cs = rd32(d, n, 0x60), co = rd32(d, n, 0x64);
    for (uint32_t ci = 0; ci < cs; ci++) {
        size_t o = (size_t) co + 32u * ci;
        if (o + 32 > n) break;
        uint32_t sv = rd32(d, n, o + 28);
        if (!sv) continue;
        size_t p = sv;
        uint32_t cnt = ep_uleb(d, n, &p);
        for (uint32_t k = 0; k < cnt && p < n; k++) {
            uint8_t h = d[p++];
            uint32_t ty = h & 0x1f, arg = h >> 5;
            if (ty == 0x17) {
                uint32_t idx = 0;
                for (uint32_t j = 0; j <= arg && p + j < n; j++)
                    idx |= (uint32_t) d[p + j] << (8 * j);
                p += arg + 1;
                char *s = ep_type_name_string(d, n, idx);
                if (s && ep_is_fqcn(s)) sl_add(out, s);
                free(s);
            } else if (ty == 0x23 || ty == 0x24) {
                ep_uleb(d, n, &p);
            } else {
                p += arg + 1;
            }
        }
    }
}
