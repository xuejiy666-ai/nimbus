void bd_hex_add(BdHexList *l, const uint8_t *p, uint32_t pos)
{
    if (l->n == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 128;
        l->v = (BdHex *) xrealloc(l->v, (size_t) l->cap * sizeof(BdHex));
    }
    memcpy(l->v[l->n].s, p, 16);
    l->v[l->n].s[16] = 0;
    l->v[l->n].pos = pos;
    l->n++;
}
