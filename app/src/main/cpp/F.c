void ep_mm_init(EpMethodMap *m, size_t cap)
{
    size_t c = 16;
    while (c < cap * 2 && c < (1u << 21)) c <<= 1;
    m->t = (EpME *) xcalloc(c, sizeof(EpME));
    m->cap = c;
    m->n = 0;
}

void vec_init(Vec *v)
{
    v->p = NULL; v->len = 0; v->cap = 0;
}
