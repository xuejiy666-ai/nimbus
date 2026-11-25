char *bd_dex_appname(const uint8_t *b, size_t n)
{
    if (n < 0x70) return NULL;
    uint32_t so = rd32(b, n, 0x3C), to = rd32(b, n, 0x44);
    uint32_t fo = rd32(b, n, 0x54), co = rd32(b, n, 0x64), cn = rd32(b, n, 0x60);
    char *cls = NULL;

    for (uint32_t k = 0; k < cn; k++) {
        size_t o = co + 32u * k;
        uint32_t cidx = rd32(b, n, o);
        uint32_t tpos = to + 4u * cidx;
        if (tpos + 4 > n) break;
        uint32_t sidx = rd32(b, n, tpos);
        uint32_t spos = so + 4u * sidx;
        if (spos + 4 > n) break;
        size_t p = rd32(b, n, spos);
        if (p >= n) break;
        uleb_read(b, n, &p);
        size_t e = p;
        while (e < n && b[e]) e++;
        if (e - p != 28) continue;
        if (memcmp(b + p, "Lcom/sagittarius/v6/AppInfo;", 28) != 0) continue;

        uint32_t doff = rd32(b, n, o + 24);
        uint32_t sv = rd32(b, n, o + 28);
        if (!sv) break;
        uint32_t flds[64];
        int nf = 0;
        if (doff) {
            size_t q = doff;
            uint32_t sf = uleb_read(b, n, &q);
            uleb_read(b, n, &q); uleb_read(b, n, &q); uleb_read(b, n, &q);
            uint32_t cur = 0;
            for (uint32_t i = 0; i < sf && nf < 64; i++) {
                cur += uleb_read(b, n, &q);
                uleb_read(b, n, &q);
                flds[nf++] = cur;
            }
        }
        size_t q = sv;
        uint32_t cnt = uleb_read(b, n, &q);
        for (uint32_t i = 0; i < cnt && i < 64; i++) {
            if (q >= n) break;
            uint8_t h = b[q++];
            uint32_t ty = h & 0x1f, arg = h >> 5;
            char *val = NULL;
            if (ty == 0x17) {
                uint32_t sidx2 = 0;
                for (uint32_t j = 0; j <= arg && q + j < n; j++)
                    sidx2 |= ((uint32_t) b[q + j]) << (8 * j);
                q += arg + 1;
                uint32_t spos2 = so + 4u * sidx2;
                if (spos2 + 4 <= n) {
                    size_t p2 = rd32(b, n, spos2);
                    if (p2 < n) {
                        uleb_read(b, n, &p2);
                        size_t e2 = p2;
                        while (e2 < n && b[e2]) e2++;
                        val = xmalloc(e2 - p2 + 1);
                        memcpy(val, b + p2, e2 - p2);
                        val[e2 - p2] = 0;
                    }
                }
            } else if (ty == 0x1e) {
                
            } else {
                q += arg + 1;
            }
            if (i < (uint32_t) nf && val) {
                uint32_t fidx = flds[i];
                uint32_t fpos = fo + 8u * fidx + 4;
                if (fpos + 4 <= n) {
                    uint32_t nidx = rd32(b, n, fpos);
                    uint32_t npos = so + 4u * nidx;
                    if (npos + 4 <= n) {
                        size_t p3 = rd32(b, n, npos);
                        if (p3 < n) {
                            uleb_read(b, n, &p3);
                            size_t e3 = p3;
                            while (e3 < n && b[e3]) e3++;
                            if (e3 - p3 == 7 && memcmp(b + p3, "APPNAME", 7) == 0) {
                                cls = val;
                                val = NULL;
                            }
                        }
                    }
                }
            }
            free(val);
            if (cls) break;
        }
        break;
    }
    return cls;
}
