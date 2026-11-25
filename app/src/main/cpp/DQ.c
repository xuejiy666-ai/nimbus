int ep_drop_hit(const char *name)
{
    if (strncmp(name, "assets/Epic/", 12) == 0) return 1;
    if (strcmp(name, "assets/Epic_Config") == 0) return 1;
    if (strcmp(name, "assets/xposed_init") == 0) return 1;
    const char *base = strrchr(name, '/');
    base = base ? base + 1 : name;
    if (strncmp(base, "libEpic", 7) == 0 && ends_with_ci(base, ".so")) return 1;
    return 0;
}
