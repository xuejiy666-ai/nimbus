uint32_t hash_str(const char *s)
{
    uint32_t h = 2166136261u;
    while (*s) { h ^= (uint8_t) *s++; h *= 16777619u; }
    return h;
}

int e2_in(const StrList *l, const char *s)
{
    for (int i = 0; i < l->n; i++) if (strcmp(l->v[i], s) == 0) return 1;
    return 0;
}
