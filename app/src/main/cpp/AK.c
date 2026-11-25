ZipEntry *zip_find(ZipReader *z, const char *name)
{
    for (int i = 0; i < z->n; i++)
        if (strcmp(z->e[i].name, name) == 0) return &z->e[i];
    return NULL;
}

int dp_uleb_len(uint32_t v)
{
    int k = 1;
    while (v >= 0x80) { v >>= 7; k++; }
    return k;
}
