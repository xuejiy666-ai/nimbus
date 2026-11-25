char *pick_str(const char *s)
{
    if (!s || !s[0]) return NULL;
    if (strncmp(s, "###", 3) == 0) return NULL;
    return xstrdup(s);
}

void ss_free(SSet *s)
{
    for (int i = 0; i < s->cap; i++) free(s->v[i]);
    free(s->v);
    s->v = NULL; s->cap = 0; s->n = 0;
}
