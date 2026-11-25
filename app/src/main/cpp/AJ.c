void zip_close(ZipReader *z)
{
    if (!z) return;
    if (z->e) {
        for (int i = 0; i < z->n; i++) free(z->e[i].name);
        free(z->e);
    }
    if (z->f) fclose(z->f);
    memset(z, 0, sizeof(*z));
}
