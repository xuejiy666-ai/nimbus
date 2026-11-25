int zip_writer_open(ZipWriter *w, const char *path)
{
    memset(w, 0, sizeof(*w));
    w->f = fopen(path, "wb");
    if (!w->f) return -1;
    setvbuf(w->f, NULL, _IOFBF, 1u << 20);
    vec_init(&w->cd);
    w->count = 0;
    return 0;
}
