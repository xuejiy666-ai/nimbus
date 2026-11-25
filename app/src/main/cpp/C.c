int in_keep(const char *name)
{
    for (int i = 0; i < g_keep.n; i++)
        if (g_keep.v[i] && strcmp(g_keep.v[i], name) == 0) return 1;
    return 0;
}

void *xcalloc(size_t n, size_t s)
{
    void *p = calloc(n ? n : 1, s ? s : 1);
    if (!p) oom();
    return p;
}
