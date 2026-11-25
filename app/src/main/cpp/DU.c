void ep_mat(const uint8_t key[16], const uint8_t nonce[16], const char *label,
                   uint8_t out[16])
{
    static const uint8_t zero4[4] = {0, 0, 0, 0};
    uint8_t h[32];
    ep_sha_parts(key, 16, nonce, 16, (const uint8_t *) label, strlen(label), zero4, 4, h);
    memcpy(out, h, 16);
}
