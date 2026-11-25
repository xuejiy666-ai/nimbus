void bd_sbox_build(void)
{
    if (bd_sbox_ready) return;
    for (int a = 1; a < 256; a++) {
        uint8_t r = 1, base = (uint8_t) a;
        int e = 254;
        while (e) {
            if (e & 1) r = bd_mul(r, base);
            base = bd_mul(base, base);
            e >>= 1;
        }
        uint8_t v = r;
        for (int k = 1; k <= 4; k++)
            v ^= (uint8_t) (((r << k) | (r >> (8 - k))) & 0xff);
        bd_sbox[a] = (uint8_t) (v ^ 0x63);
    }
    bd_sbox[0] = 0x63;
    bd_sbox_ready = 1;
}
