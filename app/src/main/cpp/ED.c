int ep_tail(const uint8_t *outer, size_t n, EpTail *t)
{
    if (n < 32) return -1;
    const uint8_t *f = outer + n - 16;
    if (memcmp(f, "EPT3", 4) != 0) return -1;
    uint32_t total = ((uint32_t) f[8] << 24) | ((uint32_t) f[9] << 16)
                   | ((uint32_t) f[10] << 8) | f[11];
    uint32_t clen = ((uint32_t) f[12] << 24) | ((uint32_t) f[13] << 16)
                  | ((uint32_t) f[14] << 8) | f[15];
    if (total > n || total < 32) return -1;
    size_t start = n - total;
    if (start < 16) return -1;
    if ((size_t) clen + 16 > total) return -1;
    t->nonceOff = start - 16;
    t->ctOff = start;
    t->ctLen = clen;
    t->tagOff = start + clen;
    t->t4 = f + 12;
    return 0;
}
