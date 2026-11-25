int has_at(const uint8_t *b, size_t n, size_t o, const char *ascii)
{
    size_t l = strlen(ascii);
    if (!b || o + l > n) return 0;
    return memcmp(b + o, ascii, l) == 0;
}

void vec_put8(Vec *v, int b)
{
    uint8_t x = (uint8_t) b;
    vec_write(v, &x, 1);
}
