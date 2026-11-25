void bd_ctr(uint8_t *d, size_t n, const uint8_t key[16], const uint8_t nonce[12], size_t lim)
{
    uint8_t rk[176];
    bd_sbox_build();
    bd_expand(key, rk);
    size_t end = (lim < n) ? lim : n;
    for (size_t i = 0; i < end; i += 16) {
        uint8_t blk[16], ks[16];
        memcpy(blk, nonce, 12);
        uint32_t k = (uint32_t) (i / 16);
        blk[12] = (uint8_t) (k >> 24); blk[13] = (uint8_t) (k >> 16);
        blk[14] = (uint8_t) (k >> 8);  blk[15] = (uint8_t) k;
        bd_enc_block(blk, rk, ks);
        size_t m = end - i; if (m > 16) m = 16;
        for (size_t j = 0; j < m; j++) d[i + j] ^= ks[j];
    }
}
