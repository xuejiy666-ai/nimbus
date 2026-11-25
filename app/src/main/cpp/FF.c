void e2_cfg_free(E2Cfg *c)
{
    free(c->vm_name); free(c->vm_xor); free(c->vm_rc4);
    free(c->dex_dir); free(c->dex_xor); free(c->dex_rc4);
    free(c->app_name);
    free(c->axml_xor); free(c->axml_rc4);
    free(c->asset_xor); free(c->asset_rc4);
    free(c->rs_xor); free(c->rs_rc4);
    sl_clear(&c->am_k); sl_clear(&c->am_v);
    sl_clear(&c->as_k); sl_clear(&c->as_v);
    memset(c, 0, sizeof(*c));
}
