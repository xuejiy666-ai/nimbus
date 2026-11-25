void sha1_of(const uint8_t *b, size_t n, uint8_t out[20])
{
    Sha1Ctx c;
    sha1_init(&c);
    sha1_update(&c, b, n);
    sha1_final(&c, out);
}

const char *e2_skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
    return p;
}
