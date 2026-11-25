uint8_t dp_gpow(uint8_t a, int e)
{
    uint8_t r = 1;
    while (e > 0) {
        if (e & 1) r = dp_gmul(r, a);
        a = dp_gmul(a, a);
        e >>= 1;
    }
    return r;
}

void sl_add(StrList *s, const char *x)
{
    sl_add_own(s, xstrdup(x ? x : ""));
}
