char *xasprintf(const char *fmt, ...)
{
    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (n < 0) { va_end(ap2); return xstrdup(""); }
    char *s = xmalloc((size_t) n + 1);
    vsnprintf(s, (size_t) n + 1, fmt, ap2);
    va_end(ap2);
    return s;
}
