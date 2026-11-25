char *self_check(const uint8_t *buf, size_t n)
{
    size_t pos = rd16(buf, n, 2);
    int chunks = 0, elements = 0, app_count = 0;
    StrPool pool;
    int have = 0;
    Vec chain;
    vec_init(&chain);
    while (pos + 8 <= n) {
        uint32_t t = rd16(buf, n, pos);
        uint32_t sz = rd32(buf, n, pos + 4);
        if (sz < 8 || pos + sz > n) break;
        chunks++;
        if (t == TYPE_STRING_POOL) {
            have = pool_init(&pool, buf, n, pos) == 0;
        } else if (t == TYPE_START && have) {
            char *nm = pool_get(&pool, (int) rd32(buf, n, pos + 20));
            elements++;
            if (strcmp(nm, "application") == 0) app_count++;
            if (chain.len < 90) {
                if (chain.len) vec_put8(&chain, '>');
                vec_write(&chain, nm, strlen(nm));
            }
            free(nm);
        }
        pos += sz;
    }
    vec_put8(&chain, 0);
    char *s = xmalloc(256 + chain.len);
    snprintf(s, 256 + chain.len, "chunks=%d elems=%d application=%d chain=%s len=%zu",
             chunks, elements, app_count, (char *) chain.p, n);
    vec_free(&chain);
    return s;
}
