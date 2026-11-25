void ep_entry_factory(EpSupMap *sup, StrList *cands, const char *pkg,
                             char **entry, char **factory)
{
    static const char *APP = "Landroid/app/Application;";
    static const char *ACF = "Landroid/app/AppComponentFactory;";
    *entry = NULL;
    *factory = NULL;

    if (cands && cands->n) {
        char **v = (char **) xmalloc(sizeof(char *) * (size_t) cands->n);
        int nv = 0;
        for (int i = 0; i < cands->n; i++) v[nv++] = cands->v[i];
        qsort(v, (size_t) nv, sizeof(char *), ep_cmp_str);

        for (int i = 0; i < nv; i++) {
            size_t len = strlen(v[i]) + 3;
            char *desc = (char *) xmalloc(len);
            snprintf(desc, len, "L%s;", v[i]);
            for (char *p = desc; *p; p++) if (*p == '.') *p = '/';
            if (strcmp(ep_sup_get(sup, desc), "") == 0 && !sup->n) { free(desc); continue; }
            int present = 0;
            for (size_t k = 0; k < sup->cap; k++)
                if (sup->t[k].used && strcmp(sup->t[k].k, desc) == 0) { present = 1; break; }
            if (!present) { free(desc); continue; }
            if (!*entry && ep_chain(sup, desc, APP)) *entry = xstrdup(v[i]);
            if (!*factory && ep_chain(sup, desc, ACF)) *factory = xstrdup(v[i]);
            free(desc);
            if (*entry && *factory) break;
        }
        free(v);
    }

    if (!*entry || !*factory) {
        char **apps = NULL, **acfs = NULL;
        int na = 0, nac = 0;
        for (size_t k = 0; k < sup->cap; k++) {
            if (!sup->t[k].used) continue;
            const char *desc = sup->t[k].k;
            if (strcmp(desc, APP) == 0 || strcmp(desc, ACF) == 0) continue;
            if (!*entry && ep_chain(sup, desc, APP)) {
                apps = (char **) xrealloc(apps, sizeof(char *) * (size_t) (na + 1));
                apps[na++] = sup->t[k].k;
            }
            if (!*factory && ep_chain(sup, desc, ACF)) {
                acfs = (char **) xrealloc(acfs, sizeof(char *) * (size_t) (nac + 1));
                acfs[nac++] = sup->t[k].k;
            }
        }
        const char *pref = NULL;
        char pfx[512];
        if (pkg && pkg[0]) {
            snprintf(pfx, sizeof(pfx), "L%s/", pkg);
            for (char *p = pfx; *p; p++) if (*p == '.') *p = '/';
            pref = pfx;
        }
        #define EP_PICK(arr, cnt, dst) do { \
            char **own = NULL; int no = 0; \
            for (int q = 0; q < (cnt); q++) \
                if (pref && strncmp((arr)[q], pref, strlen(pref)) == 0) { \
                    own = (char **) xrealloc(own, sizeof(char *) * (size_t) (no + 1)); \
                    own[no++] = (arr)[q]; } \
            char **pool = no ? own : (arr); int np = no ? no : (cnt); \
            if (np > 0) { \
                char *best = xstrdup(pool[0]); \
                for (int q = 1; q < np; q++) if (strcmp(pool[q], best) < 0) { free(best); best = xstrdup(pool[q]); } \
                size_t bl = strlen(best); \
                (dst) = (char *) xmalloc(bl); \
                memcpy((dst), best + 1, bl - 2); (dst)[bl - 2] = 0; \
                for (char *p = (dst); *p; p++) if (*p == '/') *p = '.'; \
                free(best); \
            } \
            free(own); \
        } while (0)
        if (!*entry && na) EP_PICK(apps, na, *entry);
        if (!*factory && nac) EP_PICK(acfs, nac, *factory);
        free(apps);
        free(acfs);
    }

    if (!*factory && cands) {
        for (int i = 0; i < cands->n; i++) {
            size_t l = strlen(cands->v[i]);
            if (l > 16 && strcmp(cands->v[i] + l - 16, "ComponentFactory") == 0) {
                *factory = xstrdup(cands->v[i]);
                break;
            }
        }
    }
    if (!*entry && cands) {
        for (int i = 0; i < cands->n; i++) {
            if (*factory && strcmp(cands->v[i], *factory) == 0) continue;
            size_t l = strlen(cands->v[i]);
            if (l > 16 && strcmp(cands->v[i] + l - 16, "ComponentFactory") == 0) continue;
            *entry = xstrdup(cands->v[i]);
            break;
        }
    }
    if (*entry && strcmp(*entry, "android.app.Application") == 0) { free(*entry); *entry = NULL; }
    if (*factory && strcmp(*factory, "android.app.AppComponentFactory") == 0) { free(*factory); *factory = NULL; }
}
