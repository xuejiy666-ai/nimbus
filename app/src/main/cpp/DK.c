int bd_xor_crack(Buf *blobs, int nb, StrList *mds, ZipReader *z, char alg[17], char mode[17])
{
    for (int bi = 0; bi < nb; bi++) {
        const uint8_t *b = blobs[bi].p;
        size_t n = blobs[bi].len;
        if (n > (1u << 20)) n = 1u << 20;
        if (n < 0x20000) continue;

        size_t cap = 1u << 16;
        while (cap < n * 2 && cap < (1u << 21)) cap <<= 1;
        BdWin2 *tab = (BdWin2 *) xcalloc(cap, sizeof(BdWin2));
        size_t mask = cap - 1;

        for (size_t i = 0; i + 8 <= n; i++) {
            uint64_t k = bd_rd64(b + i);
            size_t h = (size_t) ((k * 0x9e3779b97f4a7c15ull) >> 40) & mask;
            for (size_t pr = 0; pr < cap; pr++) {
                BdWin2 *w = &tab[h];
                if (w->cnt == 0) {
                    w->k = k; w->first = (uint32_t) i; w->last = (uint32_t) i;
                    w->cnt = 1; w->phases = (uint8_t) (1u << (i & 7));
                    break;
                }
                if (w->k == k) {
                    w->last = (uint32_t) i;
                    if (w->cnt < 60000) w->cnt++;
                    w->phases |= (uint8_t) (1u << (i & 7));
                    break;
                }
                h = (h + 1) & mask;
            }
        }

        size_t nw = 0;
        for (size_t i = 0; i < cap; i++) if (tab[i].cnt > 40) nw++;
        if (nw == 0) { free(tab); continue; }
        BdWin2 *wins = (BdWin2 *) xmalloc(nw * sizeof(BdWin2));
        size_t k2 = 0;
        for (size_t i = 0; i < cap; i++) {
            BdWin2 *w = &tab[i];
            if (w->cnt <= 40) continue;
            uint8_t ph = w->phases;
            int bits = 0;
            for (int q = 0; q < 8; q++) if (ph & (1u << q)) bits++;
            if (bits != 1) continue;
            uint64_t kk = w->k;
            int distinct = 0;
            for (int q = 1; q < 8; q++)
                if (((kk >> (8 * q)) & 0xff) != (kk & 0xff)) { distinct = 1; break; }
            if (!distinct) continue;
            wins[k2++] = *w;
        }
        free(tab);
        if (k2 == 0) { free(wins); continue; }
        qsort(wins, k2, sizeof(BdWin2), bd_cmp_win2);
        if (k2 > 800) k2 = 800;

        BdCand *cs = (BdCand *) xmalloc(k2 * sizeof(BdCand));
        size_t nc = 0;
        for (size_t i = 0; i < k2; i++) {
            int sh = (int) ((8 - (wins[i].first & 7)) & 7);
            uint8_t wz[8];
            bd_rot_key(wins[i].k, sh, wz);
            size_t a = wins[i].first > 64 ? wins[i].first - 64 : 0;
            size_t e2 = (size_t) wins[i].last + 72; if (e2 > n) e2 = n;
            size_t ln = e2 - a;
            if (ln < 16) continue;
            uint8_t *tmp = (uint8_t *) xmalloc(ln);
            size_t off = a & 7;
            for (size_t t = 0; t < ln; t++)
                tmp[t] = (uint8_t) (b[a + t] ^ wz[(t + off) & 7]);
            cs[nc].score = (double) bd_printable_ratio(tmp, ln) / 1000.0;
            cs[nc].k = wins[i].k;
            cs[nc].first = wins[i].first;
            cs[nc].last = wins[i].last;
            nc++;
            free(tmp);
        }
        free(wins);
        if (nc == 0) { free(cs); continue; }
        qsort(cs, nc, sizeof(BdCand), bd_cmp_cand);

        int tried = 0;
        uint8_t triedk[8][8];
        for (size_t i = 0; i < nc && tried < 8; i++) {
            uint8_t wz[8];
            bd_rot_key(cs[i].k, (int) ((8 - (cs[i].first & 7)) & 7), wz);
            int dup = 0;
            for (int j = 0; j < tried; j++)
                if (!memcmp(triedk[j], wz, 8)) { dup = 1; break; }
            if (dup) continue;
            memcpy(triedk[tried], wz, 8);
            tried++;
            uint8_t *raw = (uint8_t *) xmalloc(n);
            for (size_t t = 0; t < n; t++) raw[t] = (uint8_t) (b[t] ^ wz[t & 7]);
            BdHexList lx;
            memset(&lx, 0, sizeof(lx));
            bd_hex_scan(raw, n, &lx);
            int hit = bd_match(raw, n, &lx, mds, z, alg, mode);
            free(lx.v);
            free(raw);
            if (hit) { free(cs); return 1; }
        }
        free(cs);
    }
    return 0;
}
