int dex_idx_open(DexIdx *x, const uint8_t *d, size_t n)
{
    memset(x, 0, sizeof(*x));
    x->d = d;
    x->n = n;

    
    uint32_t ns = rd32(d, n, 0x38);
    uint32_t so = rd32(d, n, 0x3c);
    if (ns > 1000000) { ns = 0; }
    x->ns = ns;
    x->soff = xcalloc(ns ? ns : 1, sizeof(uint32_t));
    x->str = xcalloc(ns ? ns : 1, sizeof(char *));
    for (uint32_t i = 0; i < ns; i++) x->str[i] = xstrdup("");
    for (uint32_t i = 0; i < ns; i++) {
        size_t off = (size_t) so + 4u * i;
        if (so == 0 || off + 4 > n) continue;
        uint32_t p = rd32(d, n, off);
        if (p == 0 || (size_t) p >= n) continue;
        size_t pos = p;
        uleb_read(d, n, &pos);           
        size_t e = pos;
        while (e < n && d[e] != 0) e++;
        free(x->str[i]);
        x->str[i] = xmalloc(e - pos + 1);
        if (e > pos) memcpy(x->str[i], d + pos, e - pos);
        x->str[i][e - pos] = 0;
    }

    
    uint32_t nt = rd32(d, n, 0x40);
    uint32_t to = rd32(d, n, 0x44);
    if (nt > 1000000) { nt = 0; }
    x->nt = nt;
    x->tidx = xcalloc(nt ? nt : 1, sizeof(uint32_t));
    x->typ = xcalloc(nt ? nt : 1, sizeof(char *));
    for (uint32_t i = 0; i < nt; i++) {
        x->typ[i] = "";
        size_t off = (size_t) to + 4u * i;
        if (to == 0 || off + 4 > n) continue;
        uint32_t idx = rd32(d, n, off);
        x->tidx[i] = idx;
        if (idx < ns) x->typ[i] = x->str[idx];
    }

    
    uint32_t np = rd32(d, n, 0x48);
    uint32_t po = rd32(d, n, 0x4c);
    if (np > 1000000) { np = 0; }
    x->np = np;
    x->pret = xcalloc(np ? np : 1, sizeof(char *));
    x->ppar = xcalloc(np ? np : 1, sizeof(char **));
    x->pparn = xcalloc(np ? np : 1, sizeof(int));
    for (uint32_t i = 0; i < np; i++) {
        x->pret[i] = "";
        size_t base = (size_t) po + 12u * i;
        if (po == 0 || base + 12 > n) continue;
        uint32_t ret = rd32(d, n, base + 4);
        if (ret < nt) x->pret[i] = x->typ[ret];
        uint32_t pl = rd32(d, n, base + 8);
        if (pl == 0 || (size_t) pl + 4 > n) continue;
        uint32_t cnt = rd32(d, n, pl);
        if (cnt > 255 || (size_t) pl + 4 + 2 * (size_t) cnt > n) continue;
        x->pparn[i] = (int) cnt;
        x->ppar[i] = xcalloc(cnt ? cnt : 1, sizeof(char *));
        for (uint32_t j = 0; j < cnt; j++) {
            uint32_t ti = rd16(d, n, pl + 4 + 2 * j);
            x->ppar[i][j] = (ti < nt) ? x->typ[ti] : "";
        }
    }

    
    uint32_t nm = rd32(d, n, 0x58);
    uint32_t mo = rd32(d, n, 0x5c);
    if (nm > 1000000) { nm = 0; }
    x->nm = nm;
    x->mcls = xcalloc(nm ? nm : 1, sizeof(uint16_t));
    x->mproto = xcalloc(nm ? nm : 1, sizeof(uint16_t));
    x->mname = xcalloc(nm ? nm : 1, sizeof(uint32_t));
    for (uint32_t i = 0; i < nm; i++) {
        size_t base = (size_t) mo + 8u * i;
        if (mo == 0 || base + 8 > n) continue;
        x->mcls[i] = rd16(d, n, base);
        x->mproto[i] = rd16(d, n, base + 2);
        x->mname[i] = rd32(d, n, base + 4);
    }

    
    uint32_t nd = rd32(d, n, 0x60);
    uint32_t co = rd32(d, n, 0x64);
    if (nd > 1000000) { nd = 0; }
    x->nd = nd;
    x->cidx = xcalloc(nd ? nd : 1, sizeof(uint32_t));
    x->cdoff = xcalloc(nd ? nd : 1, sizeof(uint32_t));
    x->svoff = xcalloc(nd ? nd : 1, sizeof(uint32_t));
    for (uint32_t i = 0; i < nd; i++) {
        size_t base = (size_t) co + 32u * i;
        if (co == 0 || base + 32 > n) continue;
        x->cidx[i] = rd32(d, n, base);
        x->cdoff[i] = rd32(d, n, base + 24);
        x->svoff[i] = rd32(d, n, base + 28);
    }
    idx_hash_build(x);
    return 0;
}
