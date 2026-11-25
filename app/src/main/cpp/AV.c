void container_blocks_free(CBlock *b, int n)
{
    if (!b) return;
    for (int i = 0; i < n; i++) { free(b[i].name); free(b[i].data.p); }
    free(b);
}

void hits_free(Hit *h, int n)
{
    if (!h) return;
    for (int i = 0; i < n; i++) free(h[i].dex.p);
    free(h);
}
