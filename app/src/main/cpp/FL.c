void e2_jmap(const char *j, const char *key, StrList *ks, StrList *vs)
{
    sl_init(ks); sl_init(vs);
    const char *p = e2_find_key(j, key);
    if (!p || *p != '{') return;
    p = e2_skip_ws(p + 1);
    while (*p && *p != '}') {
        if (*p != '"') { p++; continue; }
        const char *k0 = p + 1, *q = k0;
        while (*q && *q != '"') { if (*q == '\\' && q[1]) q++; q++; }
        if (*q != '"') return;
        char *k = e2_unescape(k0, q);
        p = e2_skip_ws(q + 1);
        if (*p != ':') { free(k); return; }
        p = e2_skip_ws(p + 1);
        if (*p != '"') { free(k); return; }
        const char *v0 = p + 1;
        q = v0;
        while (*q && *q != '"') { if (*q == '\\' && q[1]) q++; q++; }
        if (*q != '"') { free(k); return; }
        char *v = e2_unescape(v0, q);
        sl_add(ks, k); sl_add(vs, v);
        free(k); free(v);
        p = e2_skip_ws(q + 1);
        if (*p == ',') p = e2_skip_ws(p + 1);
    }
}
