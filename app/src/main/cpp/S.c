char *sl_join(const StrList *s, char sep)
{
    if (!s || s->n == 0) return xstrdup("");
    size_t len = 0;
    for (int i = 0; i < s->n; i++) len += strlen(s->v[i]) + 1;
    char *out = xmalloc(len + 1);
    out[0] = 0;
    for (int i = 0; i < s->n; i++) {
        if (i) { size_t l = strlen(out); out[l] = sep; out[l + 1] = 0; }
        strcat(out, s->v[i]);
    }
    return out;
}
