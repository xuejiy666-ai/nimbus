void ep_init(void)
{
    if (ep_ready) return;
    bd_sbox_build();
    for (int i = 0; i < 256; i++) ep_isbox[bd_sbox[i]] = (uint8_t) i;
    ep_ready = 1;
}

int bd_cmp_ord(const void *a, const void *b)
{
    return ((const BdOrd *) a)->idx - ((const BdOrd *) b)->idx;
}
