int dp_elf_sym(const uint8_t *d, size_t n, const char *want, uint8_t out[16])
{
    if (n < 64 || memcmp(d, "\177ELF", 4) != 0) return 0;
    int is64 = d[4] == 2;
    uint64_t shoff; int shentsize, shnum, shstrndx;
    if (is64) {
        shoff = dp_rd64(d, n, 40);
        shentsize = rd16(d, n, 58); shnum = rd16(d, n, 60); shstrndx = rd16(d, n, 62);
    } else {
        shoff = rd32(d, n, 32);
        shentsize = rd16(d, n, 46); shnum = rd16(d, n, 48); shstrndx = rd16(d, n, 50);
    }
    if (!shoff || shnum <= 0 || shnum > 512 || shstrndx >= shnum) return 0;
    uint64_t nameoff[512], addr[512], off[512], size[512];
    int cnt = 0;
    for (int i = 0; i < shnum; i++) {
        uint64_t o = shoff + (uint64_t) i * (uint64_t) shentsize;
        if (o + 40 > n) return 0;
        uint32_t nm = rd32(d, n, o + 0);
        if (is64) {
            addr[cnt] = dp_rd64(d, n, o + 16);
            off[cnt] = dp_rd64(d, n, o + 24);
            size[cnt] = dp_rd64(d, n, o + 32);
            nameoff[cnt] = nm;
        } else {
            addr[cnt] = rd32(d, n, o + 12);
            off[cnt] = rd32(d, n, o + 16);
            size[cnt] = rd32(d, n, o + 20);
            nameoff[cnt] = nm;
        }
        cnt++;
    }
    uint64_t stro = off[shstrndx];
    char sname[512][40];
    for (int i = 0; i < cnt; i++) {
        size_t p = (size_t) (stro + nameoff[i]);
        sname[i][0] = 0;
        for (int k = 0; k < 39 && p + (size_t) k < n && d[p + k]; k++) sname[i][k] = (char) d[p + k];
        sname[i][39] = 0;
    }
    for (int i = 0; i < cnt; i++) {
        if (!(strcmp(sname[i], ".dynsym") == 0 || strcmp(sname[i], ".symtab") == 0)) continue;
        uint64_t strtab = 0;
        int strtab_ok = 0;
        for (int k = 0; k < cnt; k++) {
            const char *wantstr = (strcmp(sname[i], ".dynsym") == 0) ? ".dynstr" : ".strtab";
            if (strcmp(sname[k], wantstr) == 0) { strtab = off[k]; strtab_ok = 1; break; }
        }
        if (!strtab_ok) continue;
        uint64_t ents = is64 ? 24 : 16;
        uint64_t num = size[i] / ents;
        for (uint64_t k = 0; k < num; k++) {
            uint64_t eo = off[i] + k * ents;
            if (eo + ents > n) break;
            uint32_t no = rd32(d, n, eo);
            if (!no) continue;
            size_t p = (size_t) (strtab + no);
            char nm[64];
            int j = 0;
            for (; j < 63 && p + (size_t) j < n && d[p + j]; j++) nm[j] = (char) d[p + j];
            nm[j] = 0;
            if (strcmp(nm, want) != 0) continue;
            uint64_t val, shndx;
            if (is64) { shndx = rd16(d, n, eo + 6); val = dp_rd64(d, n, eo + 8); }
            else { val = rd32(d, n, eo + 4); shndx = rd16(d, n, eo + 14); }
            if (shndx >= (uint64_t) cnt) return 0;
            uint64_t fo = off[shndx] + val - addr[shndx];
            if (fo + 16 > n) return 0;
            memcpy(out, d + fo, 16);
            return 1;
        }
    }
    return 0;
}
