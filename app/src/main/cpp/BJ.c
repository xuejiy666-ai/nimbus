int prefix_match(const StrList *pref, const char *name)
{
    if (!pref || !name) return 0;
    for (int i = 0; i < pref->n; i++) {
        const char *p = pref->v[i];
        size_t l = strlen(p);
        if (l && strncmp(name, p, l) == 0) return 1;
    }
    return 0;
}
