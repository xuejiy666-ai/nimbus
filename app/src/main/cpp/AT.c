size_t cut_from(size_t n, size_t from, uint32_t len, size_t *out_len)
{
    if (len == 0 || from >= n) { *out_len = 0; return from; }
    size_t to = from + len;
    if (to > n) to = n;
    *out_len = to - from;
    return from;
}
