void bd_rot_key(uint64_t k, int s, uint8_t out[8])
{
    uint8_t w[8];
    for (int i = 0; i < 8; i++) w[i] = (uint8_t) (k >> (8 * i));
    for (int i = 0; i < 8; i++) out[i] = w[(i + s) & 7];
}

void vec_pad4(Vec *v)
{
    while (v->len % 4) vec_put8(v, 0);
}
