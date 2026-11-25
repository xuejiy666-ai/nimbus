void sc_apk_base(const char *apkPath, char *out, size_t cap)
{
    const char *bs = strrchr(apkPath, '/');
    snprintf(out, cap, "%s", bs ? bs + 1 : apkPath);
    char *cut = strrchr(out, '.');
    if (cut && cut != out) *cut = 0;
}
