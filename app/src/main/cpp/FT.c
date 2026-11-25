int e2_cfg_parse(const char *j, E2Cfg *c)
{
    int has;
    memset(c, 0, sizeof(*c));
    c->vm_name = e2_jstr(j, "vm_data_name");
    c->vm_method = e2_jint(j, "vm_protection_method", 1, &has);
    c->vm_xor = e2_jraw(j, "vm_protection_xor_key");
    c->vm_rc4 = e2_jstr(j, "vm_protection_rc4_key");
    c->dex_dir = e2_jstr(j, "dex_dir");
    if (!c->dex_dir) c->dex_dir = xstrdup("EPIC");
    c->dex_method = e2_jint(j, "dex_protection_method", 1, &has);
    c->dex_rc4 = e2_jstr(j, "dex_protection_rc4_key");
    c->dex_xor = e2_jraw(j, "dex_protection_xor_key");
    c->app_name = e2_jstr(j, "application_name");
    e2_jmap(j, "activity_map", &c->am_k, &c->am_v);
    c->has_axml = 0;
    c->axml_method = e2_jint(j, "axml_protection_method", -1, &c->has_axml);
    c->axml_xor = e2_jraw(j, "axml_protection_xor_key");
    c->axml_rc4 = e2_jstr(j, "axml_protection_rc4_key");
    c->has_asset = 0;
    c->asset_method = e2_jint(j, "asset_protection_method", 0, &c->has_asset);
    c->asset_xor = e2_jraw(j, "asset_protection_xor_key");
    c->asset_rc4 = e2_jstr(j, "asset_protection_rc4_key");
    e2_jmap(j, "asset_rename_map", &c->as_k, &c->as_v);
    c->has_rs = 0;
    c->rs_method = e2_jint(j, "resource_string_protection_method", 0, &c->has_rs);
    c->rs_xor = e2_jraw(j, "resource_string_protection_xor_key");
    c->rs_rc4 = e2_jstr(j, "resource_string_protection_rc4_key");
    return 1;
}
