char *ep_json_str(const char *js, const char *key)
{
    const char *p = strstr(js, key);
    if (!p) return NULL;
    p += strlen(key);
    while (*p && *p != ':') p++;
    if (!*p) return NULL;
    p++;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    if (*p != '"') return NULL;
    p++;
    size_t n = 0;
    while (p[n] && p[n] != '"') n++;
    char *s = (char *) xmalloc(n + 1);
    memcpy(s, p, n);
    s[n] = 0;
    return s;
}
