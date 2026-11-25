int pool_init(StrPool *p, const uint8_t *b, size_t n, size_t pool_pos)
{
    memset(p, 0, sizeof(*p));
    if (pool_pos + 28 > n) return -1;
    p->b = b;
    p->n = n;
    p->pool = pool_pos;
    p->cnt = rd32(b, n, pool_pos + 8);
    uint32_t flags = rd32(b, n, pool_pos + 16);
    p->str_start = rd32(b, n, pool_pos + 20);
    p->off_arr = (uint32_t) (pool_pos + rd16(b, n, pool_pos + 2));
    p->utf8 = (flags & 0x100) != 0;
    if (p->cnt > 1000000) p->cnt = 0;
    return 0;
}
