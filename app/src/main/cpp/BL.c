char *pkg_of(const char *desc)
{
    if (!desc) return xstrdup("");
    const char *slash = strrchr(desc, '/');
    if (!slash) return xstrdup(desc);
    size_t l = (size_t) (slash - desc) + 1;
    char *s = xmalloc(l + 1);
    memcpy(s, desc, l);
    s[l] = 0;
    return s;
}
