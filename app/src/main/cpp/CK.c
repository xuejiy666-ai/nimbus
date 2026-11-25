void package_key(const char *pkg, uint8_t key[16])
{
    size_t l = pkg ? strlen(pkg) : 0;
    for (int i = 0; i < 16; i++)
        key[i] = (uint8_t) (KEY_PAD[i] ^ (i < (int) l ? (uint8_t) pkg[i] : 0));
}
