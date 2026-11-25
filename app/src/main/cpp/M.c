uint32_t ep_hash(const char *s)
{
    uint32_t h = 2166136261u;
    while (*s) { h ^= (uint8_t) *s++; h *= 16777619u; }
    return h;
}

char *vec_str(Vec *v)
{
    char *s = xmalloc(v->len + 1);
    if (v->len) memcpy(s, v->p, v->len);
    s[v->len] = 0;
    return s;
}
