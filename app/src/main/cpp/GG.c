void dp_write_uleb(uint8_t *b, size_t *o, uint32_t v)
{
    for (;;) {
        uint8_t c = (uint8_t) (v & 0x7f);
        v >>= 7;
        if (v) { b[(*o)++] = (uint8_t) (c | 0x80); }
        else { b[(*o)++] = c; break; }
    }
}
