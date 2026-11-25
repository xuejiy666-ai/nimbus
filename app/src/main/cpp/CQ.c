uint8_t bd_mul(uint8_t a, uint8_t b)
{
    uint8_t r = 0;
    while (b) {
        if (b & 1) r ^= a;
        a = (uint8_t) ((a & 0x80) ? ((a << 1) ^ 0x1b) : (a << 1));
        b >>= 1;
    }
    return r;
}
