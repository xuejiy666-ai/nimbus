void bd_hex_scan(const uint8_t *b, size_t n, BdHexList *l)
{
    for (size_t i = 0; i + 17 <= n; i++) {
        int ok = 1;
        for (int k = 0; k < 16; k++) if (!bd_is_lhex(b[i + k])) { ok = 0; break; }
        if (!ok || b[i + 16] != 0) continue;
        if (i > 0 && bd_is_lhex(b[i - 1])) continue;
        bd_hex_add(l, b + i, (uint32_t) i);
    }
}
