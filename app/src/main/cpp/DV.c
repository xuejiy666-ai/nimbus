void ep_stream(uint8_t *data, size_t n, const uint8_t key[16],
                      const uint8_t nonce[16], const char *label)
{
    for (size_t off = 0; off < n; off += 32) {
        uint8_t be[4] = {(uint8_t) ((off / 32) >> 24), (uint8_t) ((off / 32) >> 16),
                         (uint8_t) ((off / 32) >> 8), (uint8_t) (off / 32)};
        uint8_t blk[32];
        ep_sha_parts(key, 16, nonce, 16, (const uint8_t *) label, strlen(label), be, 4, blk);
        size_t m = n - off < 32 ? n - off : 32;
        for (size_t i = 0; i < m; i++) data[off + i] ^= blk[i];
    }
}
