void hit_add(HitSet *hs, int bi, size_t off, const uint8_t *key, int klen,
                    const uint8_t *dex, size_t len)
{
    uint8_t sha[20];
    sha1_of(dex, len, sha);
    char *h = hex_of(sha, 20);
    if (sl_contains(&hs->seen, h)) { free(h); return; }
    sl_add_own(&hs->seen, h);
    if (hs->n == hs->cap) {
        hs->cap = hs->cap ? hs->cap * 2 : 8;
        hs->v = xrealloc(hs->v, (size_t) hs->cap * sizeof(Hit));
    }
    Hit *t = &hs->v[hs->n++];
    memset(t, 0, sizeof(*t));
    t->block = bi;
    t->off = off;
    t->keylen = klen > 32 ? 32 : klen;
    if (klen > 0) memcpy(t->key, key, (size_t) t->keylen);
    t->dex.p = xmalloc(len ? len : 1);
    memcpy(t->dex.p, dex, len);
    t->dex.len = len;
}
