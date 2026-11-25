int dp_insn_kind(int op)
{
    if (op == 0x1A || op == 0x1B) return DPK_NONE;
    if ((op >= 0x1C && op <= 0x1D) || op == 0x1F || op == 0x20 || op == 0x22 ||
        op == 0x23 || op == 0x24 || op == 0x25) return DPK_TYPE;
    if (op >= 0x52 && op <= 0x6D) return DPK_FIELD;
    if ((op >= 0x6E && op <= 0x78) || op == 0xFA || op == 0xFB ||
        op == 0xFC || op == 0xFD) return DPK_METHOD;
    return DPK_NONE;
}
