int dex_trim(const uint8_t *in, size_t n, const StrList *prefixes, const StrList *also, Buf *out)
{
    uint8_t *b = xmalloc(n);
    memcpy(b, in, n);
    int dropped = dex_drop_classes(b, n, prefixes, also);
    (void) dropped;
    if (rd32(b, n, 0x60) == 0) { free(b); return 0; }
    uint32_t end = rd32(b, n, 0x6c) + rd32(b, n, 0x68);
    size_t len = n;
    if (end > 0 && (size_t) end < n) {
        end = (end + 3) & ~3u;
        if ((size_t) end < n) len = end;
    }
    wr32(b, n, 0x20, (uint32_t) len);
    dex_resign(b, len);
    out->p = xrealloc(b, len ? len : 1);
    out->len = len;
    return 1;
}
