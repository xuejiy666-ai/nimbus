int ep_mm_get(EpMethodMap *m, uint32_t k, uint32_t *v)
{
    size_t h = (size_t) ((k * 2654435761u) >> 8) & (m->cap - 1);
    size_t guard = 0;
    while (m->t[h].used && guard++ < m->cap) {
        if (m->t[h].k == k) { *v = m->t[h].v; return 1; }
        h = (h + 1) & (m->cap - 1);
    }
    return 0;
}
