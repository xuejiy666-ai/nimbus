int block_scan(CBlock *blocks, int nb, const uint8_t *keys, int keylen, int nkeys,
               Hit **out, int *nout)
{
    *out = NULL;
    *nout = 0;
    HitSet hs;
    memset(&hs, 0, sizeof(hs));
    sl_init(&hs.seen);
    int winner = 1 << 30;   

    for (int bi = 0; bi < nb; bi++) {
        const uint8_t *d = blocks[bi].data.p;
        size_t n = blocks[bi].data.len;

        
        long pi = find_bytes(d, n, (const uint8_t *) "dex\n03", 6, 0);
        while (pi >= 0) {
            size_t p = (size_t) pi;
            uint32_t size = (p + 0x24 <= n) ? rd32(d, n, p + 0x20) : 0;
            if (size >= 0x70 && (size_t) size <= n - p && dex_valid(d, p, size)) {
                hit_add(&hs, bi, p, NULL, 0, d + p, size);
                long next = find_bytes(d, n, (const uint8_t *) "dex\n03", 6,
                                       p + (size > 4 ? size : 4));
                if (next < 0) break;
                pi = next;
                continue;
            }
            pi = find_bytes(d, n, (const uint8_t *) "dex\n03", 6, p + 4);
        }

        
        for (int ki = 0; ki < nkeys; ki++) {
            const uint8_t *k = keys + (size_t) ki * (size_t) keylen;
            uint8_t ks[8];
            rc4_stream(k, (size_t) keylen, ks, 8);
            for (int mi = 0; mi < 4; mi++) {
                uint8_t needle[8];
                for (int t = 0; t < 8; t++) needle[t] = (uint8_t) (MAGICS[mi][t] ^ ks[t]);
                long i = find_bytes(d, n, needle, 8, 0);
                while (i >= 0) {
                    size_t pos = (size_t) i;
                    uint32_t size = 0;
                    if (pos + 0x24 <= n) {
                        uint8_t head[0x24];
                        rc4_crypt(k, (size_t) keylen, d + pos, head, 0x24);
                        if (has_at(head, sizeof(head), 0, "dex\n")) size = rd32(head, sizeof(head), 0x20);
                    }
                    if (size < 0x70 || (size_t) size > n - pos) {
                        i = find_bytes(d, n, needle, 8, pos + 1);
                        continue;
                    }

                    uint8_t *plain = NULL;
                    int used = 0;
                    int order[7];
                    int on = 0;
                    if (winner != (1 << 30)) order[on++] = winner;
                    for (int s = 0; s < 6; s++)
                        if (SCHEMES[s] != winner) order[on++] = SCHEMES[s];
                    for (int oi = 0; oi < on; oi++) {
                        int sc = order[oi];
                        uint8_t *cand = decrypt_scheme(k, keylen, d, pos, size, sc);
                        if (dex_valid(cand, 0, size)) {
                            plain = cand;
                            used = sc;
                            break;
                        }
                        free(cand);
                    }
                    if (!plain) {
                        i = find_bytes(d, n, needle, 8, pos + 1);
                        continue;
                    }
                    winner = used;
                    hit_add(&hs, bi, pos, k, keylen, plain, size);
                    free(plain);
                    i = find_bytes(d, n, needle, 8, pos + 1);
                }
            }
        }
    }
    *out = hs.v;
    *nout = hs.n;
    sl_clear(&hs.seen);
    return 0;
}
