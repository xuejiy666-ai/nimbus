uint32_t ep_mix(uint32_t w0, uint32_t w1, uint32_t w2)
{
    uint32_t w8 = ((w0 ^ w1) >> 19) | ((w0 ^ w1) << 13);
    w8 += w2;
    w8 = (w8 >> 25) | (w8 << 7);
    uint32_t w10 = 0xF42B8CB7u;
    return (w8 ^ w10) ^ (w10 ^ w0);
}
