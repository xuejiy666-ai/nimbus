char *e2_jstr(const char *j, const char *key)
{
    const char *p = e2_find_key(j, key);
    if (!p || *p != '"') return NULL;
    const char *s = p + 1, *q = s;
    while (*q && *q != '"') { if (*q == '\\' && q[1]) q++; q++; }
    if (*q != '"') return NULL;
    return e2_unescape(s, q);
}
