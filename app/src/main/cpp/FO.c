char *e2_decode_ep(const char *val, int method, const char *xork, const char *rc4k)
{
    if (!val || strncmp(val, "EP_", 3) != 0) return NULL;
    uint8_t *enc = NULL;
    size_t en = 0;
    e2_b64(val + 3, &enc, &en);
    if (!en) { free(enc); return NULL; }
    uint8_t *pl = (uint8_t *) xmalloc(en + 1);
    if (method == 0) {
        int k = e2_xor_key(xork);
        for (size_t i = 0; i < en; i++) pl[i] = enc[i] ^ (uint8_t) k;
    } else {
        if (!rc4k) { free(enc); free(pl); return NULL; }
        rc4_crypt((const uint8_t *) rc4k, strlen(rc4k), enc, pl, en);
    }
    pl[en] = 0;
    free(enc);
    return (char *) pl;
}
