int dp_try_cfg(const uint8_t *ak, const uint8_t *cf, size_t cn, const char *km,
                      char **out, uint8_t key[32], int *klen)
{
    if (cn < 16 || (cn & 15)) return 0;
    uint8_t iv[16];
    memcpy(iv, ak, 16);
    iv[3] = 0x2F; iv[9] = 0x76;
    uint8_t *pt = (uint8_t *) xmalloc(cn);
    int ok = 0;
    for (int mode = 0; mode < 2 && !ok; mode++) {
        const uint8_t *k = ak;
        int kl = 16;
        uint8_t kk[32];
        if (mode == 1) {
            if (!km || !*km) continue;
            bd_hmac(ak, 16, (const uint8_t *) km, strlen(km), kk);
            k = kk; kl = 32;
        }
        if (dp_aes_cbc_dec(k, kl, iv, cf, cn, pt) != 0) continue;
        int pad = pt[cn - 1];
        if (pad < 1 || pad > 16) continue;
        size_t m = cn - (size_t) pad;
        char *t = (char *) xmalloc(m + 1);
        memcpy(t, pt, m);
        t[m] = 0;
        if (strstr(t, "app_name") || strstr(t, "jni_cls_name") || strstr(t, "acf_name") ||
            strstr(t, "insns_xor_key") || strstr(t, "junk_cls_name")) {
            *out = t;
            memcpy(key, k, (size_t) kl);
            *klen = kl;
            ok = 1;
        } else free(t);
    }
    free(pt);
    return ok;
}
