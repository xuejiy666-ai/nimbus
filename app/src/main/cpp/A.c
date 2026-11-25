void vec_uleb(Vec *v, uint32_t x)
{
    for (;;) {
        uint8_t b = (uint8_t) (x & 0x7f);
        x >>= 7;
        if (x) vec_put8(v, b | 0x80);
        else { vec_put8(v, b); return; }
    }
}

void oom(void)
{
    abort();
}
