void ep_dec_block(const uint8_t in[16], const uint8_t rk[176], uint8_t out[16])
{
    uint8_t st[16], o[16];
    for (int i = 0; i < 16; i++) st[i] = (uint8_t) (in[i] ^ rk[160 + i]);
    for (int r = 9; r >= 1; r--) {
        memcpy(o, st, 16);
        for (int row = 0; row < 4; row++)
            for (int c = 0; c < 4; c++)
                st[4 * c + row] = o[4 * ((c - row + 4) & 3) + row];
        for (int i = 0; i < 16; i++) st[i] = ep_isbox[st[i]];
        for (int i = 0; i < 16; i++) st[i] = (uint8_t) (st[i] ^ rk[16 * r + i]);
        for (int c = 0; c < 4; c++) {
            uint8_t a0 = st[4 * c], a1 = st[4 * c + 1], a2 = st[4 * c + 2], a3 = st[4 * c + 3];
            st[4 * c]     = (uint8_t) (bd_mul(a0, 14) ^ bd_mul(a1, 11) ^ bd_mul(a2, 13) ^ bd_mul(a3, 9));
            st[4 * c + 1] = (uint8_t) (bd_mul(a0, 9) ^ bd_mul(a1, 14) ^ bd_mul(a2, 11) ^ bd_mul(a3, 13));
            st[4 * c + 2] = (uint8_t) (bd_mul(a0, 13) ^ bd_mul(a1, 9) ^ bd_mul(a2, 14) ^ bd_mul(a3, 11));
            st[4 * c + 3] = (uint8_t) (bd_mul(a0, 11) ^ bd_mul(a1, 13) ^ bd_mul(a2, 9) ^ bd_mul(a3, 14));
        }
    }
    memcpy(o, st, 16);
    for (int row = 0; row < 4; row++)
        for (int c = 0; c < 4; c++)
            st[4 * c + row] = o[4 * ((c - row + 4) & 3) + row];
    for (int i = 0; i < 16; i++) st[i] = ep_isbox[st[i]];
    for (int i = 0; i < 16; i++) out[i] = (uint8_t) (st[i] ^ rk[i]);
}
