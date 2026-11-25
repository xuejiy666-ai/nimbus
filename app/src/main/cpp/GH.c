const uint8_t *dp_memmem(const uint8_t *h, size_t hn, const char *nd, size_t nn)
{
    if (!hn || !nn || nn > hn) return NULL;
    for (size_t i = 0; i + nn <= hn; i++) if (memcmp(h + i, nd, nn) == 0) return h + i;
    return NULL;
}
