int ep_inflate_dex(const uint8_t *d, size_t n, Buf *out)
{
    if (n > 8 && memcmp(d + n - 8, "EPICHIDE", 8) == 0) n -= 8;
    int wb[3] = {-15, 15, 47};
    for (int i = 0; i < 3; i++) {
        Buf t = {NULL, 0};
        if (ep_inflate_w(d, n, wb[i], &t) == 0) {
            if (ep_dex_ok(t.p, t.len)) { *out = t; return 0; }
            free(t.p);
        }
    }
    return -1;
}
