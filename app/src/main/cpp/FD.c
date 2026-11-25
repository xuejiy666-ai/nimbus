void sc_out_dir(char *work, size_t cap, const char *outDir, const char *apkPath,
                       const char *pkg, const char *root, const char *sub)
{
    char nm[256];
    (void) root;
    (void) sub;
    sc_apk_base(apkPath, nm, sizeof(nm));
    const char *tail = (pkg && pkg[0]) ? pkg : nm;
    snprintf(work, cap, "%s/%s", outDir, tail);
    mkdir_p(work);
}
