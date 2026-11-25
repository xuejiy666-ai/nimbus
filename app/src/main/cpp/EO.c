const char *ep_sup_get(EpSupMap *m, const char *k)
{
    uint32_t h = ep_hash(k);
    size_t i = h & (m->cap - 1);
    size_t g = 0;
    while (m->t[i].used && g++ < m->cap) {
        if (m->t[i].h == h && strcmp(m->t[i].k, k) == 0) return m->t[i].v;
        i = (i + 1) & (m->cap - 1);
    }
    return "";
}
