char *pool_get(StrPool *p, int idx)
{
    if (idx < 0 || (uint32_t) idx >= p->cnt) return xstrdup("");
    uint32_t o = rd32(p->b, p->n, p->off_arr + 4u * (uint32_t) idx);
    return decode_at(p->b, p->n, p->pool + p->str_start + o, p->utf8);
}
