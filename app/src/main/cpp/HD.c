double dp_match_score(DpInsn *ins, int nins, DpCode *codes, int nc)
{
    if (!nins || !nc) return 0.0;
    int ok = 0, valid = 0;
    for (int i = 0; i < nins; i++) {
        for (int k = 0; k < nc; k++) {
            if (codes[k].mi != ins[i].mi) continue;
            valid++;
            if (codes[k].bytes == ins[i].len) ok++;
            break;
        }
    }
    return valid ? (double) ok / (double) valid : 0.0;
}
