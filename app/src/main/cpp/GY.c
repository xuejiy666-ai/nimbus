int dp_parse_pool(const uint8_t *d, size_t n, int *ver, DpPoolDex **out, int *ocnt)
{
    if (n < 4) return -1;
    uint32_t v = rd16(d, n, 0);
    uint32_t cnt = rd16(d, n, 2);
    if (v != 1 && v != 2) return -1;
    if (cnt == 0 || cnt > 16) return -1;
    size_t o = 4 + (size_t) cnt * 4;
    DpPoolDex *arr = (DpPoolDex *) xmalloc(sizeof(DpPoolDex) * cnt);
    memset(arr, 0, sizeof(DpPoolDex) * cnt);
    int made = 0;
    for (uint32_t i = 0; i < cnt; i++) {
        if (o + 2 > n) goto bad;
        uint32_t mc = rd16(d, n, o);
        o += 2;
        if (mc > 65535) goto bad;
        arr[i].v = (DpInsn *) xmalloc(sizeof(DpInsn) * (mc ? mc : 1));
        arr[i].n = 0;
        for (uint32_t k = 0; k < mc; k++) {
            if (o + 8 > n) goto bad;
            uint32_t mi = rd32(d, n, o);
            uint32_t sz;
            if (v == 1) {
                if (o + 12 > n) goto bad;
                sz = rd32(d, n, o + 8);
                o += 12;
            } else {
                sz = rd32(d, n, o + 4);
                o += 8;
            }
            if (o + sz > n) goto bad;
            arr[i].v[arr[i].n].mi = mi;
            arr[i].v[arr[i].n].p = d + o;
            arr[i].v[arr[i].n].len = sz;
            arr[i].n++;
            o += sz;
        }
        made++;
    }
    *ver = (int) v;
    *out = arr;
    *ocnt = made;
    return 0;
bad:
    for (int i = 0; i < made; i++) free(arr[i].v);
    free(arr);
    return -1;
}
