int e2_xor_key(const char *raw)
{
    if (!raw) return 0;
    while (*raw == ' ' || *raw == '\t') raw++;
    if (!*raw) return 0;
    if (raw[0] == '0' && (raw[1] == 'x' || raw[1] == 'X'))
        return (int) strtol(raw + 2, NULL, 16) & 0xff;
    int neg = 0;
    const char *q = raw;
    if (*q == '-') { neg = 1; q++; }
    if (*q >= '0' && *q <= '9') {
        long v = strtol(raw, NULL, 10);
        (void) neg;
        return (int) v & 0xff;
    }
    return ((unsigned char) raw[0]) & 0xff;
}
