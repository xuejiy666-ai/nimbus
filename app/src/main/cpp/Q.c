int sl_contains(const StrList *s, const char *x)
{
    if (!x) return 0;
    for (int i = 0; i < s->n; i++)
        if (s->v[i] && strcmp(s->v[i], x) == 0) return 1;
    return 0;
}

int bd_is_lhex(int c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'); }
