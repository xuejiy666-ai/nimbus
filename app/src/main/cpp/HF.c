void dpt_ctx_free(DptCtx *c)
{
    for (int i = 0; i < c->nd; i++) free(c->dexes[i].p);
    free(c->dexes);
    dp_pool_free(c->pool, c->npool);
    free(c->praw.p);
    free(c->inner.p);
    free(c->so.p);
    free(c->cf.p);
    free(c->manifest.p);
    free(c->pkg);
    free(c->verm);
    free(c->cfg);
    zip_close(&c->z);
}
