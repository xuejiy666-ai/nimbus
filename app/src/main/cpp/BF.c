uint8_t *decrypt_scheme(const uint8_t *k, int klen, const uint8_t *d, size_t off,
                               size_t size, int scheme)
{
    uint8_t *out = xmalloc(size ? size : 1);
    if (scheme == 0) {
        rc4_crypt(k, (size_t) klen, d + off, out, size);
        return out;
    }
    if (scheme == -1 || scheme == -3) {
        size_t h = (scheme == -1) ? 0x20000 : 0x70;
        if (h > size) h = size;
        if (h) rc4_crypt(k, (size_t) klen, d + off, out, h);
        if (size > h) memcpy(out + h, d + off + h, size - h);
        return out;
    }
    if (scheme == -2) {
        size_t h = 0x70;
        if (h > size) h = size;
        if (h) memcpy(out, d + off, h);
        if (size > h) rc4_crypt(k, (size_t) klen, d + off + h, out + h, size - h);
        return out;
    }
    size_t chunk = (scheme == 1) ? 0x10000 : 0x20000;
    for (size_t p = 0; p < size; p += chunk) {
        size_t m = (size - p) < chunk ? (size - p) : chunk;
        rc4_crypt(k, (size_t) klen, d + off + p, out + p, m);
    }
    return out;
}
