void bd_sha_final(BdSha *c, uint8_t out[32])
{
    uint64_t bits = c->len * 8;
    uint8_t pad = 0x80;
    bd_sha_update(c, &pad, 1);
    uint8_t z = 0;
    while (c->n != 56) bd_sha_update(c, &z, 1);
    uint8_t lb[8];
    for (int i = 0; i < 8; i++) lb[i] = (uint8_t) (bits >> (56 - 8 * i));
    bd_sha_update(c, lb, 8);
    for (int i = 0; i < 8; i++) {
        out[4 * i] = (uint8_t) (c->h[i] >> 24);
        out[4 * i + 1] = (uint8_t) (c->h[i] >> 16);
        out[4 * i + 2] = (uint8_t) (c->h[i] >> 8);
        out[4 * i + 3] = (uint8_t) c->h[i];
    }
}
