void ep_sup_init(EpSupMap *m, size_t cap)
{
    size_t c = 64;
    while (c < cap * 2) c <<= 1;
    m->t = (EpSup *) xcalloc(c, sizeof(EpSup));
    m->cap = c;
    m->n = 0;
}

uint8_t rd8(const uint8_t *b, size_t n, size_t o)
{
    return (b && o < n) ? b[o] : 0;
}
