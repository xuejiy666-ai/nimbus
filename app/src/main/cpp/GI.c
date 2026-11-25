uint8_t dp_gmul(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        uint8_t hi = (uint8_t) (a & 0x80);
        a = (uint8_t) (a << 1);
        if (hi) a ^= 0x1b;
        b = (uint8_t) (b >> 1);
    }
    return p;
}
