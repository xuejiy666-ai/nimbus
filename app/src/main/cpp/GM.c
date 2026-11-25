int dp_aes_cbc_dec(const uint8_t *key, int klen, const uint8_t *iv,
                          const uint8_t *in, size_t n, uint8_t *out)
{
    if (n == 0 || n % 16) return -1;
    if (klen != 16 && klen != 24 && klen != 32) return -1;
    dp_sb_init();
    uint8_t rk[240];
    int nr = 0;
    dp_aes_expand(key, klen, rk, &nr);
    uint8_t prev[16], blk[16];
    memcpy(prev, iv, 16);
    for (size_t o = 0; o < n; o += 16) {
        dp_aes_dec_block(in + o, blk, rk, nr);
        for (int i = 0; i < 16; i++) out[o + i] = (uint8_t) (blk[i] ^ prev[i]);
        memcpy(prev, in + o, 16);
    }
    return 0;
}
