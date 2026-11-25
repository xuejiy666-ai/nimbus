void ep_sup_free(EpSupMap *m)
{
    for (size_t i = 0; i < m->cap; i++)
        if (m->t[i].used) { free(m->t[i].k); free(m->t[i].v); }
    free(m->t);
    m->t = NULL;
}

int e2_starts(const char *s, const char *pre)
{
    return strncmp(s, pre, strlen(pre)) == 0;
}
