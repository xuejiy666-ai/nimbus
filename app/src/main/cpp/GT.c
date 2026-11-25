int dp_ay_build_keys(const uint8_t *d, size_t n, char out[][33], int max)
{
    int nout = dp_ay_anchor_keys(d, n, out, max);

    int ncpu = (int) sysconf(_SC_NPROCESSORS_ONLN);
    if (ncpu < 1) ncpu = 1;
    if (ncpu > 8) ncpu = 8;
    if (n < 65536) ncpu = 1;
    if (ncpu == 1) {
        for (int line = 1; line <= 1400 && nout < max; line++)
            dp_ay_scan_key(d, n, dp_ay_gen_key(line), out, &nout, max);
        return nout;
    }

    DpKeyJob job[8];
    pthread_t th[8];
    int made[8];
    int per = 1400 / ncpu;
    for (int t = 0; t < ncpu; t++) {
        job[t].d = d; job[t].n = n; job[t].max = 600; job[t].cnt = 0;
        job[t].out = (char (*)[33]) xmalloc(33 * 600);
        job[t].lo = t * per + 1;
        job[t].hi = (t == ncpu - 1) ? 1400 : (t + 1) * per;
        made[t] = pthread_create(&th[t], NULL, dp_key_job, &job[t]) == 0;
        if (!made[t]) dp_key_job(&job[t]);
    }
    for (int t = 0; t < ncpu; t++)
        if (made[t]) pthread_join(th[t], NULL);

    for (int t = 0; t < ncpu; t++) {
        for (int k = 0; k < job[t].cnt && nout < max; k++) {
            int dup = 0;
            for (int q = 0; q < nout; q++)
                if (strcmp(out[q], job[t].out[k]) == 0) { dup = 1; break; }
            if (!dup) snprintf(out[nout++], 33, "%s", job[t].out[k]);
        }
        free(job[t].out);
    }
    return nout;
}
