int bd_is_stub_class(const char *cur)
{
    static const char *pfx[] = {
        "com.SecShell.SecShell.",
        "com.sagittarius.v6.",
        "com.baidu.protect.",
        "epic.",
        "com.epic.",
        "com.baidu.appquery.",
        NULL
    };
    if (!cur) return 0;
    for (int i = 0; pfx[i]; i++)
        if (strncmp(cur, pfx[i], strlen(pfx[i])) == 0) return 1;
    return 0;
}
