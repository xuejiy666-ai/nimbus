void bd_enc_block(const uint8_t in[16], const uint8_t rk[176], uint8_t out[16])
{
    uint8_t st[4][4], t[4][4];
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            st[r][c] = (uint8_t) (in[4 * c + r] ^ rk[4 * c + r]);
    for (int rnd = 1; rnd <= 10; rnd++) {
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++)
                st[r][c] = bd_sbox[st[r][c]];
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++)
                t[r][c] = st[r][(c + r) & 3];
        memcpy(st, t, 16);
        if (rnd != 10) {
            for (int c = 0; c < 4; c++) {
                uint8_t a0 = st[0][c], a1 = st[1][c], a2 = st[2][c], a3 = st[3][c];
                st[0][c] = (uint8_t) (bd_mul(a0, 2) ^ bd_mul(a1, 3) ^ a2 ^ a3);
                st[1][c] = (uint8_t) (a0 ^ bd_mul(a1, 2) ^ bd_mul(a2, 3) ^ a3);
                st[2][c] = (uint8_t) (a0 ^ a1 ^ bd_mul(a2, 2) ^ bd_mul(a3, 3));
                st[3][c] = (uint8_t) (bd_mul(a0, 3) ^ a1 ^ a2 ^ bd_mul(a3, 2));
            }
        }
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                st[r][c] = (uint8_t) (st[r][c] ^ rk[rnd * 16 + 4 * c + r]);
    }
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            out[4 * c + r] = st[r][c];
}
