long find_bytes(const uint8_t *h, size_t hn, const uint8_t *nd, size_t nn, size_t from)
{
    if (!h || !nd || !nn || hn < nn) return -1;
    for (size_t i = from; i + nn <= hn; i++)
        if (h[i] == nd[0] && memcmp(h + i, nd, nn) == 0) return (long) i;
    return -1;
}
