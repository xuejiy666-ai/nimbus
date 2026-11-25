int dp_dex_has_class(const uint8_t *d, size_t n, const char *dotcls)
{
    char want[256];
    snprintf(want, sizeof(want), "L%s;", dotcls);
    for (size_t i = 0; i + strlen(want) < n; i++) {
        if (d[i] == 'L' && memcmp(d + i, want, strlen(want)) == 0) return 1;
    }
    return 0;
}
