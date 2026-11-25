int bd_hit_name(const char *s)
{
    size_t n = strlen(s);
    for (size_t i = 0; i + 11 <= n; i++) {
        int k;
        for (k = 0; k < 11; k++) {
            char a = s[i + k], b = "baiduprotect"[k];
            if (a >= 'A' && a <= 'Z') a = (char) (a + 32);
            if (a != b) break;
        }
        if (k == 11) return 1;
    }
    return 0;
}
