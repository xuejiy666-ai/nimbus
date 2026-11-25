void *dex_pool_run(void *arg)
{
    DexPool *p = (DexPool *) arg;
    for (;;) {
        pthread_mutex_lock(&p->mu);
        int i = (p->next < p->n) ? p->next++ : -1;
        pthread_mutex_unlock(&p->mu);
        if (i < 0) break;
        dex_job_run(&p->jobs[i]);
    }
    return NULL;
}
