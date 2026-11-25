void bd_hmac(const uint8_t *key, size_t klen, const uint8_t *msg, size_t mlen, uint8_t out[32])
{
    uint8_t k[64], ip[64], op[64];
    memset(k, 0, 64);
    if (klen > 64) {
        BdSha t; bd_sha_init(&t); bd_sha_update(&t, key, klen); bd_sha_final(&t, k);
    } else {
        memcpy(k, key, klen);
    }
    for (int i = 0; i < 64; i++) { ip[i] = (uint8_t) (k[i] ^ 0x36); op[i] = (uint8_t) (k[i] ^ 0x5c); }
    BdSha c;
    bd_sha_init(&c); bd_sha_update(&c, ip, 64); bd_sha_update(&c, msg, mlen);
    uint8_t ih[32]; bd_sha_final(&c, ih);
    bd_sha_init(&c); bd_sha_update(&c, op, 64); bd_sha_update(&c, ih, 32);
    bd_sha_final(&c, out);
}
