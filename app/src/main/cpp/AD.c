void sha1_update(Sha1Ctx *c, const uint8_t *b, size_t n)
{
    if (!n) return;
    c->total += n;
    size_t i = 0;
    if (c->buflen) {
        size_t need = 64 - c->buflen;
        size_t take = n < need ? n : need;
        memcpy(c->buf + c->buflen, b, take);
        c->buflen += take;
        i += take;
        if (c->buflen == 64) { sha1_block(c, c->buf); c->buflen = 0; }
    }
    while (i + 64 <= n) { sha1_block(c, b + i); i += 64; }
    if (i < n) {
        memcpy(c->buf, b + i, n - i);
        c->buflen = n - i;
    }
}
