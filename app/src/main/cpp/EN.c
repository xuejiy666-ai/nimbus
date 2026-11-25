void ep_sup_put(EpSupMap *m, const char *k, const char *v)
{
    if (m->n * 4 > m->cap * 3) {
        EpSup *old = m->t;
        size_t oc = m->cap;
        ep_sup_init(m, oc);
        for (size_t i = 0; i < oc; i++)
            if (old[i].used) ep_sup_put(m, old[i].k, old[i].v);
        for (size_t i = 0; i < oc; i++) if (old[i].used) { free(old[i].k); free(old[i].v); }
        free(old);
    }
    uint32_t h = ep_hash(k);
    size_t i = h & (m->cap - 1);
    while (m->t[i].used) {
        if (m->t[i].h == h && strcmp(m->t[i].k, k) == 0) { free(m->t[i].v); m->t[i].v = xstrdup(v); return; }
        i = (i + 1) & (m->cap - 1);
    }
    m->t[i].used = 1;
    m->t[i].h = h;
    m->t[i].k = xstrdup(k);
    m->t[i].v = xstrdup(v);
    m->n++;
}
