int ep_epc2(const uint8_t *blob, size_t n, const uint8_t key[16], Buf *cfg,
                   EpBlock **blocks, int *nblocks)
{
    EpBlock *v = NULL;
    int nv = 0;
    if (n < 28 || memcmp(blob, "EPC2", 4) != 0) return -1;
    const uint8_t *nonce = blob + 4;
    uint32_t s1 = ((uint32_t) blob[20] << 24) | ((uint32_t) blob[21] << 16)
                | ((uint32_t) blob[22] << 8) | blob[23];
    uint32_t s2 = ((uint32_t) blob[24] << 24) | ((uint32_t) blob[25] << 16)
                | ((uint32_t) blob[26] << 8) | blob[27];
    if ((size_t) 28 + s1 + s2 != n) return -1;
    uint8_t mk[16], iv[16];
    ep_mat(key, nonce, "cfg", mk);
    ep_mat(key, nonce, "iv", iv);
    if (ep_cbc_dec(blob + 28, s1, mk, iv, cfg) != 0) return -1;
    cfg->p = (uint8_t *) xrealloc(cfg->p, cfg->len + 1);
    cfg->p[cfg->len] = 0;

    uint8_t *code = (uint8_t *) xmalloc(s2 ? s2 : 1);
    memcpy(code, blob + 28 + s1, s2);
    ep_stream(code, s2, key, nonce, "code");

    if (s2 < 4) { free(code); free(cfg->p); cfg->p = NULL; return -1; }
    uint32_t cnt = rd16(code, s2, 2);
    if (4 + 4 * (size_t) cnt > s2) { free(code); free(cfg->p); cfg->p = NULL; return -1; }
    uint32_t *offs = (uint32_t *) xmalloc(sizeof(uint32_t) * (cnt ? cnt : 1));
    for (uint32_t i = 0; i < cnt; i++) offs[i] = rd32(code, s2, 4 + 4 * i);

    for (uint32_t bi = 0; bi < cnt; bi++) {
        uint32_t bo = offs[bi];
        uint32_t lim = (bi + 1 < cnt) ? offs[bi + 1] : (uint32_t) s2;
        if ((size_t) bo + 2 > s2) goto bad;
        uint32_t rc = rd16(code, s2, bo);
        uint32_t cur = bo + 2;
        EpRec *recs = NULL;
        int nr = 0;
        for (uint32_t r = 0; r < rc; r++) {
            if ((size_t) cur + 8 > s2) { free(recs); goto bad; }
            uint32_t rid = rd32(code, s2, cur);
            uint32_t rl = rd32(code, s2, cur + 4);
            cur += 8;
            if ((size_t) cur + rl > s2) { free(recs); goto bad; }
            recs = (EpRec *) xrealloc(recs, sizeof(EpRec) * (size_t) (nr + 1));
            recs[nr].mid = rid;
            recs[nr].enc.p = (uint8_t *) xmalloc(rl ? rl : 1);
            memcpy(recs[nr].enc.p, code + cur, rl);
            recs[nr].enc.len = rl;
            nr++;
            cur += rl;
        }
        if (cur != lim) { free(recs); goto bad; }
        v = (EpBlock *) xrealloc(v, sizeof(EpBlock) * (size_t) (nv + 1));
        v[nv].recs = recs;
        v[nv].nrec = nr;
        nv++;
    }
    free(offs);
    free(code);
    *blocks = v;
    *nblocks = nv;
    return 0;
bad:
    for (int i = 0; i < nv; i++) {
        for (int j = 0; j < v[i].nrec; j++) free(v[i].recs[j].enc.p);
        free(v[i].recs);
    }
    free(v);
    free(offs);
    free(code);
    free(cfg->p);
    cfg->p = NULL;
    return -1;
}
