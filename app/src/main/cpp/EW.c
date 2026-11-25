int ep_json_int(const char *js, const char *key, uint32_t *out)
{
    const char *p = strstr(js, key);
    if (!p) return 0;
    p += strlen(key);
    while (*p && *p != ':') p++;
    if (!*p) return 0;
    p++;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    int neg = 0;
    if (*p == '-') { neg = 1; p++; }
    if (*p < '0' || *p > '9') return 0;
    unsigned long v = strtoul(p, NULL, 10);
    *out = neg ? (uint32_t) (0u - (uint32_t) v) : (uint32_t) v;
    return 1;
}
