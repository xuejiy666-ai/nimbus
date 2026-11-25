uint32_t shash(const char *s, int cap)
{
    uint32_t h = 2166136261u;
    while (*s) { h ^= (uint8_t) *s++; h *= 16777619u; }
    return h & (uint32_t) (cap - 1);
}

int ep_cmp_str(const void *a, const void *b)
{
    return strcmp(*(char *const *) a, *(char *const *) b);
}
