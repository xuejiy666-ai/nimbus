int ep_key_ok(const uint8_t k[16], const uint8_t *nonce, const uint8_t *ct,
                     const uint8_t *tag, const uint8_t t4[4], size_t clen)
{
    static const uint8_t zero4[4] = {0, 0, 0, 0};
    uint8_t h[32];
    ep_sha_parts(k, 16, nonce, 16, (const uint8_t *) "dpt-tail-v1", 11, zero4, 4, h);
    if ((ct[0] ^ h[0]) != 0x50 || (ct[1] ^ h[1]) != 0x4B) return 0;
    static const uint8_t ept3[4] = {'E', 'P', 'T', '3'};
    uint8_t one = 0x01;
    BdSha s;
    bd_sha_init(&s);
    bd_sha_update(&s, k, 16);
    bd_sha_update(&s, ept3, 4);
    bd_sha_update(&s, &one, 1);
    bd_sha_update(&s, nonce, 16);
    bd_sha_update(&s, t4, 4);
    bd_sha_update(&s, ct, clen);
    uint8_t out[32];
    bd_sha_final(&s, out);
    return memcmp(out, tag, 16) == 0;
}
