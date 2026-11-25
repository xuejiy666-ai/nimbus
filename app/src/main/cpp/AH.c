int read_at(FILE *f, uint64_t off, void *buf, size_t n)
{
    if (fseek(f, (long) off, SEEK_SET) != 0) return -1;
    return fread(buf, 1, n, f) == n ? 0 : -1;
}

int is_dex_name(const char *n)
{
    return n && strncmp(n, "classes", 7) == 0 && ends_with_ci(n, ".dex");
}
