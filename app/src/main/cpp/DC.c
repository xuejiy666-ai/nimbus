void bd_kdf(const uint8_t *k1, size_t k1len, const uint8_t *msg, size_t msglen,
                   const char *mode, uint8_t key[16], uint8_t nonce[12])
{
    uint8_t s[32], o[32], m[80];
    size_t ml = strlen(mode);
    if (ml > 60) ml = 60;
    bd_hmac(k1, k1len, msg, msglen, s);
    memcpy(m, mode, ml);
    m[ml] = 0x01;
    bd_hmac(s, 32, m, ml + 1, o);
    memcpy(key, o, 16);
    memcpy(nonce, o + 16, 12);
}
