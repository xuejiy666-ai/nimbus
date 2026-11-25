char *dp_json_str(const char *j, const char *key)
{
    char pat[96];
    snprintf(pat, sizeof(pat), "\"%s\"", key);
    const char *p = strstr(j, pat);
    if (!p) return NULL;
    p += strlen(pat);
    while (*p && *p != ':') p++;
    if (!*p) return NULL;
    p++;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    if (*p != '"') return NULL;
    p++;
    const char *e = p;
    char *o = (char *) xmalloc(512);
    size_t w = 0;
    while (*e && *e != '"' && w + 1 < 512) {
        if (*e == '\\' && e[1]) e++;
        o[w++] = *e++;
    }
    o[w] = 0;
    return o;
}
