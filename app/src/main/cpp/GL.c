void dp_aes_dec_block(const uint8_t in[16], uint8_t out[16], const uint8_t *rk, int nr)
{
    uint8_t s[16];
    memcpy(s, in, 16);
    for (int i = 0; i < 16; i++) s[i] ^= rk[16 * nr + i];
    for (int round = nr - 1; round >= 0; round--) {
        uint8_t t[16];
        
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                t[r + 4 * ((c + r) & 3)] = s[r + 4 * c];
        
        for (int i = 0; i < 16; i++) s[i] = (uint8_t) (g_disb[t[i]] ^ rk[16 * round + i]);
        if (round > 0) {
            for (int c = 0; c < 4; c++) {
                uint8_t *p = s + 4 * c;
                uint8_t a0 = p[0], a1 = p[1], a2 = p[2], a3 = p[3];
                uint8_t m0 = (uint8_t) (dp_gmul(a0, 14) ^ dp_gmul(a1, 11) ^ dp_gmul(a2, 13) ^ dp_gmul(a3, 9));
                uint8_t m1 = (uint8_t) (dp_gmul(a0, 9) ^ dp_gmul(a1, 14) ^ dp_gmul(a2, 11) ^ dp_gmul(a3, 13));
                uint8_t m2 = (uint8_t) (dp_gmul(a0, 13) ^ dp_gmul(a1, 9) ^ dp_gmul(a2, 14) ^ dp_gmul(a3, 11));
                uint8_t m3 = (uint8_t) (dp_gmul(a0, 11) ^ dp_gmul(a1, 13) ^ dp_gmul(a2, 9) ^ dp_gmul(a3, 14));
                p[0] = m0; p[1] = m1; p[2] = m2; p[3] = m3;
            }
        }
    }
    memcpy(out, s, 16);
}
