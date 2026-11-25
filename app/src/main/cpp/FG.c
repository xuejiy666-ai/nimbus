const char *e2_find_key(const char *j, const char *key)
{
    char pat[160];
    snprintf(pat, sizeof(pat), "\"%s\"", key);
    const char *p = strstr(j, pat);
    if (!p) return NULL;
    p = e2_skip_ws(p + strlen(pat));
    if (*p != ':') return NULL;
    return e2_skip_ws(p + 1);
}
