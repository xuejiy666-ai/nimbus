void ep_mm_put(EpMethodMap *m, uint32_t k, uint32_t v)
{
    size_t h = (size_t) ((k * 2654435761u) >> 8) & (m->cap - 1);
    while (m->t[h].used && m->t[h].k != k) h = (h + 1) & (m->cap - 1);
    m->t[h].used = 1;
    m->t[h].k = k;
    m->t[h].v = v;
    m->n++;
}
