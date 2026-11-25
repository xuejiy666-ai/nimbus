void dex_walk(DexIdx *x, uint32_t code_off, insn_fn fn, void *ud)
{
    const uint8_t *d = x->d;
    size_t n = x->n;
    if (code_off == 0 || (size_t) code_off + 16 > n) return;
    uint32_t cnt = rd32(d, n, code_off + 12);
    size_t base = (size_t) code_off + 16;
    if (cnt > 0x200000) return;
    uint32_t k = 0;
    while (k < cnt) {
        if (base + 2 * (size_t) k + 4 > n) break;
        uint16_t u = rd16(d, n, base + 2 * (size_t) k);
        int op = u & 0xff;
        if (u == 0x0100) { k += (uint32_t) rd16(d, n, base + 2 * (size_t) k + 2) * 2 + 4; continue; }
        if (u == 0x0200) { k += (uint32_t) rd16(d, n, base + 2 * (size_t) k + 2) * 4 + 2; continue; }
        if (u == 0x0300) {
            uint32_t w = rd16(d, n, base + 2 * (size_t) k + 2);
            uint32_t sz = rd32(d, n, base + 2 * (size_t) k + 4);
            k += (uint32_t) ((w * sz + 1) / 2 + 4);
            continue;
        }
        int invoke = (op >= 0x6e && op <= 0x72) || (op >= 0x74 && op <= 0x78);
        if (invoke && base + 2 * (size_t) k + 6 <= n) {
            fn(ud, base + 2 * (size_t) k, op, WIDTH[op], rd16(d, n, base + 2 * (size_t) k + 2));
        }
        k += (uint32_t) WIDTH[op];
    }
}
