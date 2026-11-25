int ep_chain(EpSupMap *m, const char *desc, const char *want)
{
    const char *cur = desc;
    for (int i = 0; i < 24 && cur && cur[0]; i++) {
        if (strcmp(cur, want) == 0) return 1;
        cur = ep_sup_get(m, cur);
    }
    return strcmp(desc, want) == 0;
}
