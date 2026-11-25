int ep_dex_ord(const char *nm)
{
    if (strcmp(nm, "classes.dex") == 0) return 0;
    int v = 0;
    for (const char *p = nm + 7; *p >= '0' && *p <= '9'; p++) v = v * 10 + (*p - '0');
    return v;
}
