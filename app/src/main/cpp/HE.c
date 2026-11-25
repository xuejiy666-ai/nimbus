double dp_insn_score(DpInsn *ins, int nins, int mode, const uint8_t *aeskey, int klen)
{
    int good = 0, tot = 0;
    for (int i = 0; i < nins && tot < 200; i++) {
        const uint8_t *b = ins[i].p;
        uint32_t len = ins[i].len;
        if (len < 2) continue;
        tot++;
        int last;
        if (mode == 0) last = b[len - 2];
        else if (mode == 1) last = b[len - 2];
        else {
            
            uint8_t *tmp = (uint8_t *) xmalloc(len);
            uint8_t k[36];
            memcpy(k, aeskey, (size_t) klen);
            k[klen] = (uint8_t) (ins[i].mi); k[klen + 1] = (uint8_t) (ins[i].mi >> 8);
            k[klen + 2] = (uint8_t) (ins[i].mi >> 16); k[klen + 3] = (uint8_t) (ins[i].mi >> 24);
            rc4_crypt(k, (size_t) klen + 4, b, tmp, len);
            last = tmp[len - 2];
            free(tmp);
        }
        if (dp_return_op(last)) good++;
    }
    return tot ? (double) good / (double) tot : 0.0;
}
