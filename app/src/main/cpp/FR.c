Buf e2_elf_section(const uint8_t *d, size_t n, const char *want)
{
    Buf r = {NULL, 0};
    if (n < 64 || d[0] != 0x7f || d[1] != 'E' || d[2] != 'L' || d[3] != 'F') return r;
    int is32 = d[4] == 1;
    uint64_t shoff; int shnum, shstr;
    if (is32) {
        shoff = rd32(d, n, 32); shnum = rd16(d, n, 48); shstr = rd16(d, n, 50);
    } else {
        shoff = e2_rd64(d, n, 40); shnum = rd16(d, n, 60); shstr = rd16(d, n, 62);
    }
    int shsz = is32 ? 40 : 64;
    if (shoff + (uint64_t) shstr * shsz + shsz > n) return r;
    uint64_t stroff = is32 ? rd32(d, n, (size_t)(shoff + (uint64_t) shstr * shsz + 16))
                           : e2_rd64(d, n, (size_t)(shoff + (uint64_t) shstr * shsz + 24));
    for (int i = 0; i < shnum; i++) {
        uint64_t e = shoff + (uint64_t) i * shsz;
        if (e + shsz > n) break;
        uint32_t ni = rd32(d, n, (size_t) e);
        uint64_t np = stroff + ni;
        if (np >= n) continue;
        if (strcmp((const char *) d + np, want) != 0) continue;
        uint64_t off = is32 ? rd32(d, n, (size_t) e + 16) : e2_rd64(d, n, (size_t) e + 24);
        uint64_t sz = is32 ? rd32(d, n, (size_t) e + 20) : e2_rd64(d, n, (size_t) e + 32);
        if (off + sz > n) return r;
        r.p = (uint8_t *) xmalloc(sz ? (size_t) sz : 1);
        memcpy(r.p, d + off, (size_t) sz);
        r.len = (size_t) sz;
        return r;
    }
    return r;
}
