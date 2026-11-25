void dp_sb_init(void)
{
    if (g_dsb_ready) return;
    for (int x = 0; x < 256; x++) {
        uint8_t inv = x ? dp_gpow((uint8_t) x, 254) : 0;
        uint8_t s = inv, r = inv;
        for (int i = 0; i < 4; i++) {
            s = (uint8_t) (((s << 1) | (s >> 7)) & 0xff);
            r ^= s;
        }
        g_dsb[x] = (uint8_t) (r ^ 0x63);
    }
    for (int i = 0; i < 256; i++) g_disb[g_dsb[i]] = (uint8_t) i;
    g_dsb_ready = 1;
}
