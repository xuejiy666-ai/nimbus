Buf build_pool(Buf *spans, int nspans, BufList *added, uint32_t flags)
{
    int n = nspans + added->n;
    Vec data;
    vec_init(&data);
    uint32_t *offs = xcalloc((size_t) n ? (size_t) n : 1, sizeof(uint32_t));
    for (int i = 0; i < n; i++) {
        offs[i] = (uint32_t) data.len;
        Buf b = (i < nspans) ? spans[i] : added->v[i - nspans];
        vec_write(&data, b.p, b.len);
    }
    vec_pad4(&data);
    uint32_t strings_start = 28 + 4u * (uint32_t) n;
    uint32_t size = strings_start + (uint32_t) data.len;

    Vec o;
    vec_init(&o);
    vec_put16(&o, TYPE_STRING_POOL);
    vec_put16(&o, 28);
    vec_put32(&o, size);
    vec_put32(&o, (uint32_t) n);
    vec_put32(&o, 0);
    vec_put32(&o, flags & 0x100);
    vec_put32(&o, strings_start);
    vec_put32(&o, 0);
    for (int i = 0; i < n; i++) vec_put32(&o, offs[i]);
    vec_write(&o, data.p, data.len);
    free(offs);
    vec_free(&data);
    Buf b = {o.p, o.len};
    return b;
}
