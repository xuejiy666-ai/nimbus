void sha1_init(Sha1Ctx *c)
{
    c->h[0] = 0x67452301u; c->h[1] = 0xEFCDAB89u; c->h[2] = 0x98BADCFEu;
    c->h[3] = 0x10325476u; c->h[4] = 0xC3D2E1F0u;
    c->buflen = 0; c->total = 0;
}

void vec_free(Vec *v)
{
    free(v->p);
    v->p = NULL; v->len = v->cap = 0;
}
