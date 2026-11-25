int is_shell_entry(const char *name)
{
    size_t n = strlen(name);
    for (size_t i = 0; i + 8 <= n; i++) {
        int k;
        for (k = 0; k < 8; k++) {
            char a = name[i + k], b = "SecShell"[k];
            if (a >= 'A' && a <= 'Z') a = (char) (a + 32);
            if (b >= 'A' && b <= 'Z') b = (char) (b + 32);
            if (a != b) break;
        }
        if (k == 8) return 1;
    }
    if (strncmp(name, "assets/meta-data/", 17) == 0) return 1;
    {
        size_t m = strlen(name);
        for (size_t i = 0; i + 5 <= m; i++) {
            int k;
            for (k = 0; k < 5; k++) {
                char a = name[i + k], b = "baidu"[k];
                if (a >= 'A' && a <= 'Z') a = (char) (a + 32);
                if (a != b) break;
            }
            if (k == 5) return 1;
        }
    }
    return 0;
}
