char *attr_string(const uint8_t *buf, size_t n, size_t pos, size_t hdr,
                         const char *attr_name, StrPool *pool)
{
    uint32_t a_start = rd16(buf, n, pos + 24);
    uint32_t a_size = rd16(buf, n, pos + 26);
    uint32_t a_cnt = rd16(buf, n, pos + 28);
    for (uint32_t i = 0; i < a_cnt; i++) {
        size_t q = pos + hdr + a_start + (size_t) i * a_size;
        if (q + 20 > n) break;
        char *nm = pool_get(pool, (int) rd32(buf, n, q + 4));
        int match = strcmp(nm, attr_name) == 0;
        free(nm);
        if (!match) continue;
        if (buf[q + 15] == 0x03) return pool_get(pool, (int) rd32(buf, n, q + 16));
        return pool_get(pool, (int) rd32(buf, n, q + 8));
    }
    return NULL;
}
