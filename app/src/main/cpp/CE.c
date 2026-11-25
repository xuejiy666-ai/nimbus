int write_file(const char *path, const uint8_t *data, size_t len)
{
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    int ok = (len == 0) || (fwrite(data, 1, len, f) == len);
    fclose(f);
    return ok ? 0 : -1;
}
