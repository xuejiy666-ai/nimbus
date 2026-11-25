void *dp_key_job(void *p)
{
    DpKeyJob *j = (DpKeyJob *) p;
    for (int line = j->lo; line <= j->hi; line++) {
        if (j->cnt >= j->max) break;
        dp_ay_scan_key(j->d, j->n, dp_ay_gen_key(line), j->out, &j->cnt, j->max);
    }
    return NULL;
}
