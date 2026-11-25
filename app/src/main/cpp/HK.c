void sc_unpack_result_free(UnpackResult *r)
{
    free(r->pkg); free(r->keyHex); free(r->appName); free(r->acfName);
    free(r->restoredApk); free(r->tail); free(r->outDir);
    memset(r, 0, sizeof(*r));
}
