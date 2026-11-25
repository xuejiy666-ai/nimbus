void ep_make_work(char *work, size_t cap, const char *outDir, const char *apkPath,
                         const char *pkg, const char *variant)
{
    sc_out_dir(work, cap, outDir, apkPath, pkg, "", variant);
}
