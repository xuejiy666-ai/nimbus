void ep_sha_parts(const uint8_t *a, size_t an, const uint8_t *b, size_t bn,
                         const uint8_t *c, size_t cn, const uint8_t *d, size_t dn,
                         uint8_t out[32])
{
    BdSha s;
    bd_sha_init(&s);
    if (an) bd_sha_update(&s, a, an);
    if (bn) bd_sha_update(&s, b, bn);
    if (cn) bd_sha_update(&s, c, cn);
    if (dn) bd_sha_update(&s, d, dn);
    bd_sha_final(&s, out);
}
