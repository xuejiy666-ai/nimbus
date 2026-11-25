char *e2_jraw(const char *j, const char *key)
{
    const char *p = e2_find_key(j, key);
    if (!p) return NULL;
    if (*p == '"') return e2_jstr(j, key);
    const char *q = p;
    while (*q && *q != ',' && *q != '}' && *q != '\n' && *q != ' ' && *q != '\r' && *q != '\t') q++;
    if (q == p) return NULL;
    char *s = xmalloc((size_t)(q - p) + 1);
    memcpy(s, p, (size_t)(q - p));
    s[q - p] = 0;
    return s;
}
