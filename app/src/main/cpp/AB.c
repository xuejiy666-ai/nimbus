void rc4_crypt(const uint8_t *key, size_t klen, const uint8_t *in, uint8_t *out, size_t len)
{
    uint8_t s[256];
    for (int i = 0; i < 256; i++) s[i] = (uint8_t) i;
    if (klen == 0) klen = 1;
    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + s[i] + key[i % klen]) & 0xff;
        uint8_t t = s[i]; s[i] = s[j]; s[j] = t;
    }
    int i = 0;
    j = 0;
    for (size_t k = 0; k < len; k++) {
        i = (i + 1) & 0xff;
        j = (j + s[i]) & 0xff;
        uint8_t t = s[i]; s[i] = s[j]; s[j] = t;
        out[k] = in[k] ^ s[(s[i] + s[j]) & 0xff];
    }
}
