void sha1_final(Sha1Ctx *c, uint8_t out[20])
{
    uint64_t bits = c->total << 3;
    size_t pad = (c->buflen < 56) ? (56 - c->buflen) : (120 - c->buflen);
    uint8_t tail[128];
    memset(tail, 0, sizeof(tail));
    tail[0] = 0x80;
    for (int i = 0; i < 8; i++) tail[pad + i] = (uint8_t) (bits >> (56 - 8 * i));
    sha1_update(c, tail, pad + 8);
    for (int k = 0; k < 5; k++) {
        out[4 * k] = (uint8_t) (c->h[k] >> 24);
        out[4 * k + 1] = (uint8_t) (c->h[k] >> 16);
        out[4 * k + 2] = (uint8_t) (c->h[k] >> 8);
        out[4 * k + 3] = (uint8_t) c->h[k];
    }
}
