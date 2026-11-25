void vec_put32(Vec *v, uint32_t x)
{
    uint8_t t[4] = {(uint8_t) x, (uint8_t) (x >> 8), (uint8_t) (x >> 16), (uint8_t) (x >> 24)};
    vec_write(v, t, 4);
}

int dp_is_hexc(int c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
