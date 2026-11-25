int ep_is_fqcn(const char *s)
{
    if (!s || !*s) return 0;
    int dot = 0;
    for (const char *p = s; *p; p++) {
        if (*p == '.') { dot = 1; continue; }
        if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z')
              || (*p >= '0' && *p <= '9') || *p == '_' || *p == '$')) return 0;
    }
    if (!dot || s[0] == '.' || strchr(s, ':')) return 0;
    static const char *junk[] = {".java", ".kt", ".so", ".dex", ".apk", ".xml",
                                 ".json", ".png", ".jar", NULL};
    size_t n = strlen(s);
    for (int i = 0; junk[i]; i++) {
        size_t jn = strlen(junk[i]);
        if (n > jn && strcmp(s + n - jn, junk[i]) == 0) return 0;
    }
    return 1;
}
