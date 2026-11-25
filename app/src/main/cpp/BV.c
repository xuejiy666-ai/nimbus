char *axml_read_package(const uint8_t *buf, size_t n)
{
    size_t pos = rd16(buf, n, 2);
    StrPool pool;
    int have = 0;
    while (pos + 8 <= n) {
        uint32_t t = rd16(buf, n, pos);
        uint32_t sz = rd32(buf, n, pos + 4);
        if (sz < 8 || pos + sz > n) break;
        if (t == TYPE_STRING_POOL) {
            have = pool_init(&pool, buf, n, pos) == 0;
        } else if (t == TYPE_START && have) {
            char *nm = pool_get(&pool, (int) rd32(buf, n, pos + 20));
            int is_manifest = strcmp(nm, "manifest") == 0;
            free(nm);
            if (is_manifest) {
                uint32_t a_start = rd16(buf, n, pos + 24);
                uint32_t a_size = rd16(buf, n, pos + 26);
                uint32_t a_cnt = rd16(buf, n, pos + 28);
                for (uint32_t a = 0; a < a_cnt; a++) {
                    size_t q = pos + 16 + a_start + (size_t) a * a_size;
                    if (q + 20 > n) break;
                    uint32_t an = rd32(buf, n, q + 4);
                    uint32_t raw = rd32(buf, n, q + 8);
                    int dt = buf[q + 15];
                    uint32_t data = rd32(buf, n, q + 16);
                    char *anm = pool_get(&pool, (int) an);
                    int is_pkg = strcmp(anm, "package") == 0;
                    free(anm);
                    if (is_pkg) {
                        if (dt == 0x03 && data < pool.cnt) return pool_get(&pool, (int) data);
                        if (raw != 0 && raw < pool.cnt) return pool_get(&pool, (int) raw);
                    }
                }
            }
        }
        pos += sz;
    }
    return NULL;
}
