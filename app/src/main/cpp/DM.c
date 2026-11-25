char *bd_dotted(const char *s)
{
    if (!s) return NULL;
    size_t n = strlen(s);
    char *o = xmalloc(n + 1);
    memcpy(o, s, n + 1);
    if (n >= 2 && o[0] == 'L' && o[n - 1] == ';') {
        memmove(o, o + 1, n - 1);
        o[n - 2] = 0;
    }
    for (char *p = o; *p; p++) if (*p == '/') *p = '.';
    return o;
}
