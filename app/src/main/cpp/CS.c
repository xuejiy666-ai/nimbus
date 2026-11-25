void bd_expand(const uint8_t key[16], uint8_t rk[176])
{
    memcpy(rk, key, 16);
    uint8_t rc = 1;
    for (int i = 16; i < 176; i += 4) {
        uint8_t t[4];
        memcpy(t, rk + i - 4, 4);
        if ((i % 16) == 0) {
            uint8_t t0 = t[0];
            t[0] = (uint8_t) (bd_sbox[t[1]] ^ rc);
            t[1] = bd_sbox[t[2]];
            t[2] = bd_sbox[t[3]];
            t[3] = bd_sbox[t0];
            rc = (uint8_t) ((rc & 0x80) ? ((rc << 1) ^ 0x1b) : (rc << 1));
        }
        for (int j = 0; j < 4; j++) rk[i + j] = (uint8_t) (rk[i - 16 + j] ^ t[j]);
    }
}
