int ep_find_key(ZipReader *z, const uint8_t *cls, size_t cln, uint8_t key[16],
                       Buf *payload, int *havePayload)
{
    EpTail t;
    if (ep_tail(cls, cln, &t) != 0) return -1;
    const uint8_t *nonce = cls + t.nonceOff;
    const uint8_t *ct = cls + t.ctOff;
    const uint8_t *tag = cls + t.tagOff;
    if (t.tagOff + 16 > cln) return -1;

    int found = 0;
    for (int rev = 0; rev < 2 && !found; rev++) {
        for (int pass = 0; pass < 6 && !found; pass++) {
            for (int i = 0; i < z->n && !found; i++) {
                if (!ep_ent_order(z, pass, i)) continue;
                if (pass == 5 && (strcmp(z->e[i].name, "classes.dex") == 0)) continue;
                Buf b = {NULL, 0};
                if (zip_read(z, &z->e[i], &b) != 0) continue;
                uint8_t *alt = NULL;
                for (int style = 0; style < 2 && !found; style++) {
                    const uint8_t *base = b.p;
                    size_t bn = b.len;
                    if (style == 1) {
                        alt = (uint8_t *) xmalloc(b.len ? b.len : 1);
                        for (size_t q = 0; q < b.len; q++) alt[q] = (uint8_t) (b.p[q] ^ 0x80);
                        base = alt;
                    }
                    if (rev) {
                        for (size_t off = 0; off + 15 < bn && !found; off++) {
                            uint8_t k[16];
                            for (int j = 0; j < 16; j++) k[j] = base[off + 15 - j];
                            if (ep_key_ok(k, nonce, ct, tag, t.t4, t.ctLen)) {
                                memcpy(key, k, 16);
                                found = 1;
                            }
                        }
                    } else {
                        for (size_t off = 0; off + 15 < bn && !found; off++) {
                            if (ep_key_ok(base + off, nonce, ct, tag, t.t4, t.ctLen)) {
                                memcpy(key, base + off, 16);
                                found = 1;
                            }
                        }
                    }
                    free(alt);
                    alt = NULL;
                }
                free(b.p);
            }
        }
    }
    if (!found) return -1;

    uint8_t *pl = (uint8_t *) xmalloc(t.ctLen ? t.ctLen : 1);
    memcpy(pl, ct, t.ctLen);
    ep_stream(pl, t.ctLen, key, nonce, "dpt-tail-v1");
    if (t.ctLen < 2 || pl[0] != 'P' || pl[1] != 'K') { free(pl); return -1; }
    payload->p = pl;
    payload->len = t.ctLen;
    *havePayload = 1;
    return 0;
}
