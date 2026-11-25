char *e2_extract_json(const char *text)
{
    const char *a = strchr(text, '{');
    const char *b = strrchr(text, '}');
    if (!a || !b || b <= a) return NULL;
    char *s = xmalloc((size_t)(b - a) + 2);
    memcpy(s, a, (size_t)(b - a) + 1);
    s[b - a + 1] = 0;
    return s;
}
