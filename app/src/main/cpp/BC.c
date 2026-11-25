void dex_idx_free(DexIdx *x)
{
    if (!x) return;
    for (uint32_t i = 0; i < x->ns; i++) free(x->str[i]);
    free(x->str); free(x->soff);
    free(x->typ); free(x->tidx);
    for (uint32_t i = 0; i < x->np; i++) free(x->ppar ? x->ppar[i] : NULL);
    free(x->ppar); free(x->pparn); free(x->pret);
    free(x->mcls); free(x->mproto); free(x->mname);
    free(x->cidx); free(x->cdoff); free(x->svoff);
    free(x->hslot);
    for (int i = 0; i < x->notes_n; i++) free(x->notes[i]);
    memset(x, 0, sizeof(*x));
}
