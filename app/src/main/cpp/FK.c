int e2_jint(const char *j, const char *key, int dflt, int *has)
{
    char *s = e2_jraw(j, key);
    if (!s) { if (has) *has = 0; return dflt; }
    if (has) *has = 1;
    int v = 0;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) v = (int) strtol(s + 2, NULL, 16);
    else v = atoi(s);
    free(s);
    return v;
}
