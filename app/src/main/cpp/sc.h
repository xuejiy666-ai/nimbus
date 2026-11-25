#ifndef SC_H
#define SC_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <zlib.h>
#include <pthread.h>
#include <dlfcn.h>
#include <jni.h>

typedef struct { uint8_t *p; size_t len; } Buf;
typedef struct { uint8_t *p; size_t len, cap; } Vec;
typedef struct { char **v; int n, cap; } StrList;
void *xmalloc(size_t n);
void *xcalloc(size_t n, size_t s);
void *xrealloc(void *p, size_t n);
char *xstrdup(const char *s);
void vec_init(Vec *v);
void vec_reserve(Vec *v, size_t need);
void vec_write(Vec *v, const void *p, size_t n);
void vec_put8(Vec *v, int b);
void vec_put16(Vec *v, uint32_t x);
void vec_put32(Vec *v, uint32_t x);
void vec_uleb(Vec *v, uint32_t x);
void vec_pad4(Vec *v);
void vec_free(Vec *v);
char *vec_str(Vec *v);
void sl_init(StrList *s);
void sl_add(StrList *s, const char *x);
void sl_add_own(StrList *s, char *x);
int  sl_contains(const StrList *s, const char *x);
void sl_clear(StrList *s);
char *sl_join(const StrList *s, char sep);
uint8_t  rd8 (const uint8_t *b, size_t n, size_t o);
uint16_t rd16(const uint8_t *b, size_t n, size_t o);
uint32_t rd32(const uint8_t *b, size_t n, size_t o);
void wr16(uint8_t *b, size_t n, size_t o, uint32_t v);
void wr32(uint8_t *b, size_t n, size_t o, uint32_t v);
int  has_at(const uint8_t *b, size_t n, size_t o, const char *ascii);
long find_bytes(const uint8_t *h, size_t hn, const uint8_t *nd, size_t nn, size_t from);
uint32_t uleb_read(const uint8_t *b, size_t n, size_t *pos);
char *hex_of(const uint8_t *b, size_t n);
void rc4_crypt(const uint8_t *key, size_t klen, const uint8_t *in, uint8_t *out, size_t len);
void rc4_stream(const uint8_t *key, size_t klen, uint8_t *out, size_t n);
void sha1_of(const uint8_t *b, size_t n, uint8_t out[20]);
uint32_t adler32_of(const uint8_t *b, size_t n);
typedef struct {
    char *name;
    int method;
    uint64_t csize, usize, lho;
    uint32_t crc;
} ZipEntry;
typedef struct {
    FILE *f;
    uint64_t size;
    ZipEntry *e;
    int n;
} ZipReader;
typedef struct {
    FILE *f;
    Vec cd;
    uint64_t offset;
    int count;
} ZipWriter;
int  zip_open(const char *path, ZipReader *z);
void zip_close(ZipReader *z);
ZipEntry *zip_find(ZipReader *z, const char *name);
int  zip_read(ZipReader *z, ZipEntry *e, Buf *out);
int  zip_writer_open(ZipWriter *w, const char *path);
int  zip_writer_add(ZipWriter *w, const char *name, const uint8_t *data, size_t len);
int  zip_raw_read(ZipReader *z, ZipEntry *e, Buf *out);
int  zip_writer_add_raw(ZipWriter *w, const char *name, int method, uint32_t crc,
                        uint64_t csize, uint64_t usize, const uint8_t *data, size_t len);
int  zip_writer_close(ZipWriter *w);
typedef struct { char *name; Buf data; } CBlock;
int  container_trailer(const uint8_t *d, size_t n);
int  container_blocks(const uint8_t *d, size_t n, CBlock **out, int *count);
void container_blocks_free(CBlock *b, int n);
int  dex_valid(const uint8_t *d, size_t off, size_t len);
void dex_resign(uint8_t *d, size_t n);
char *dex_why(const uint8_t *d, size_t off, size_t len);
typedef struct {
    const uint8_t *d;
    size_t n;
    uint32_t ns; uint32_t *soff; char **str;
    uint32_t nt; uint32_t *tidx; char **typ;
    uint32_t np; char **pret; char ***ppar; int *pparn;
    uint32_t nm; uint16_t *mcls; uint16_t *mproto; uint32_t *mname;
    uint32_t nd; uint32_t *cidx; uint32_t *cdoff; uint32_t *svoff;
    int hcap; int *hslot;
    int notes_n;
    char *notes[10];
} DexIdx;
int  dex_idx_open(DexIdx *x, const uint8_t *d, size_t n);
void dex_idx_free(DexIdx *x);
int  dex_idx_class(DexIdx *x, const char *desc);
typedef void (*insn_fn)(void *ud, size_t off, int op, int width, int target);
void dex_walk(DexIdx *x, uint32_t code_off, insn_fn fn, void *ud);
typedef struct { int block; size_t off; uint8_t key[32]; int keylen; Buf dex; } Hit;
int block_scan(CBlock *blocks, int nb, const uint8_t *keys, int keylen, int nkeys,
               Hit **out, int *nout);
void hits_free(Hit *h, int n);
void dex_collect_classes(const uint8_t *d, size_t n, StrList *out);
typedef struct {
    Buf dex;
    StrList removed;
    int nops, dropped;
    StrList notes;
} CleanRes;
int  dex_drop_classes(uint8_t *b, size_t n, const StrList *prefixes, const StrList *also);
int  dex_trim(const uint8_t *in, size_t n, const StrList *prefixes, const StrList *also, Buf *out);
char *axml_read_package(const uint8_t *buf, size_t n);
typedef struct {
    int ok;
    int dexCount;
    char *pkg;
    char *keyHex;
    char *appName;
    char *acfName;
    char *restoredApk;
    char *tail;
    char *outDir;
} UnpackResult;
int  sc_unpack_run(const char *apkPath, const char *outDir, UnpackResult *r);
void sc_unpack_result_free(UnpackResult *r);
typedef struct {
    uint32_t h[5];
    uint8_t buf[64];
    size_t buflen;
    uint64_t total;
} Sha1Ctx;
extern const int8_t WIDTH[256];
extern const char *MAGICS[4];
extern const int SCHEMES[6];
typedef struct {
    Hit *v;
    int n, cap;
    StrList seen;
} HitSet;
extern StrList g_keep;
typedef struct {
    DexIdx *x;
    const char *cls;
    const char *app_prefix;
    StrList *cand;
    char **site_pkgs;
    size_t *site_offs;
    int *site_widths;
    int sn, scap;
} CleanCtx;
#define TYPE_STRING_POOL 0x0001
#define TYPE_START       0x0102
#define TYPE_END         0x0103
#define SHELL_APP "com.SecShell.SecShell.AW"
#define SHELL_ACF "com.SecShell.SecShell.AP"
typedef struct {
    const uint8_t *b;
    size_t n;
    size_t pool;
    uint32_t cnt;
    uint32_t off_arr;
    uint32_t str_start;
    int utf8;
} StrPool;
typedef struct { uint8_t **v; int n, cap; } AttrList;
typedef struct { Buf *v; int n, cap; } BufList;
typedef struct { uint32_t type, pos, size, hdr; } Chunk;
typedef struct { char **v; int cap, n; } SSet;
extern const uint8_t KEY_PAD[16];
typedef struct {
    const uint8_t *src;
    size_t slen;
    const char *pkg;
    const char *path;
    Buf plain;
    Buf z;
    uint32_t crc;
    int ok;
} DexJob;
typedef struct {
    DexJob *jobs;
    int n;
    int next;
    pthread_mutex_t mu;
} DexPool;
int bd_hit_name(const char *s);
int ep_drop_hit(const char *name);
int dp_drop_hit(const char *name);
extern uint8_t bd_sbox[256];
extern int bd_sbox_ready;
typedef struct { uint32_t h[8]; uint64_t len; uint8_t buf[64]; size_t n; } BdSha;
typedef struct { char s[17]; uint32_t pos; } BdHex;
typedef struct { BdHex *v; int n, cap; } BdHexList;
typedef struct { uint64_t k; uint32_t first, last, cnt; uint8_t phases; } BdWin;
typedef struct { double score; uint64_t k; uint32_t first, last; } BdCand;
typedef struct { uint64_t k; uint32_t first, last, cnt; uint8_t phases; } BdWin2;
typedef struct { int idx; int slot; } BdOrd;
typedef struct { char *name; Buf data; } EpDex;
typedef struct { const char *name; uint64_t size; int idx; } EpEnt;
typedef struct { uint32_t off, seed, mult, inc; } EpLcg;
typedef struct { size_t nonceOff, ctOff, tagOff, ctLen; const uint8_t *t4; } EpTail;
typedef struct { uint32_t mid; Buf enc; } EpRec;
typedef struct { EpRec *recs; int nrec; } EpBlock;
extern uint8_t ep_isbox[256];
extern int ep_ready;
typedef struct { uint32_t k, v; uint8_t used; } EpME;
typedef struct { EpME *t; size_t cap, n; } EpMethodMap;
typedef struct { char *k, *v; uint32_t h; uint8_t used; } EpSup;
typedef struct { EpSup *t; size_t cap, n; } EpSupMap;
#undef EP_PICK
typedef struct {
    char *vm_name; int vm_method; char *vm_xor; char *vm_rc4;
    char *dex_dir; int dex_method; char *dex_xor; char *dex_rc4;
    char *app_name;
    StrList am_k, am_v;
    int has_axml, axml_method; char *axml_xor, *axml_rc4;
    int has_asset, asset_method; char *asset_xor, *asset_rc4;
    StrList as_k, as_v;
    int has_rs, rs_method; char *rs_xor, *rs_rc4;
} E2Cfg;
typedef struct { const char *name; Buf d; } E2Rep;
uint32_t dp_uleb(const uint8_t *d, size_t n, size_t *p);
enum { DPK_NONE = 0, DPK_TYPE, DPK_FIELD, DPK_METHOD, DPK_STRING };
extern uint8_t dp_opw[256];
extern int dp_opw_init;
typedef int (*DpMethodFn)(void *ctx, uint32_t mi, uint32_t flags, uint32_t co, uint32_t sz);
typedef struct { const uint8_t *d; size_t n; uint8_t *patched; int count; } DpNeutCtx;
typedef struct { const uint8_t *d; size_t n; uint8_t *helpers; int fixed; } DpNopCtx;
typedef struct {
    const uint8_t *d;
    size_t n;
    char **names;
    unsigned char *cand;
    uint32_t cds;
    int hit;
} DpOrphanRef;
extern uint8_t g_dsb[256], g_disb[256];
extern int g_dsb_ready;
typedef struct {
    const uint8_t *d;
    size_t n;
    int lo, hi, max;
    char (*out)[33];
    int cnt;
} DpKeyJob;
typedef struct { uint32_t mi; const uint8_t *p; uint32_t len; } DpInsn;
typedef struct { DpInsn *v; int n; } DpPoolDex;
typedef struct { uint32_t mi; uint32_t bytes; } DpCode;
typedef struct {
    ZipReader z;
    Buf manifest, so, cf, praw, inner;
    char *pkg, *verm, *cfg;
    uint8_t ak[8][16];
    uint8_t aeskey[32];
    int nak, klen, ver, npool, have_xk, nd;
    long xk;
    DpPoolDex *pool;
    Buf *dexes;
} DptCtx;
void sc_unpack_result_free(UnpackResult *r);
#ifndef OBF_NATIVE_CLASS
#define OBF_NATIVE_CLASS "com/mint/toolbox/Native"
#endif
#ifndef OBF_STR_NAME
#define OBF_STR_NAME "str"
#endif
#ifndef OBF_RUN_NAME
#define OBF_RUN_NAME "run"
#endif
#ifndef OBF_LOG_NAME
#define OBF_LOG_NAME "log"
#endif
#ifndef OBF_STR_SIG
#define OBF_STR_SIG "(Ljava/lang/String;)Ljava/lang/String;"
#endif
#ifndef OBF_RUN_SIG
#define OBF_RUN_SIG "(Ljava/lang/String;Ljava/lang/String;Lcom/mint/toolbox/LogSink;)[Ljava/lang/String;"
#endif

void oom(void);
void *xmalloc(size_t n);
void *xcalloc(size_t n, size_t s);
void *xrealloc(void *p, size_t n);
char *xstrdup(const char *s);
void vec_init(Vec *v);
void vec_reserve(Vec *v, size_t need);
void vec_write(Vec *v, const void *p, size_t n);
void vec_put8(Vec *v, int b);
void vec_put16(Vec *v, uint32_t x);
void vec_put32(Vec *v, uint32_t x);
void vec_uleb(Vec *v, uint32_t x);
void vec_pad4(Vec *v);
void vec_free(Vec *v);
char *vec_str(Vec *v);
void sl_init(StrList *s);
void sl_add_own(StrList *s, char *x);
void sl_add(StrList *s, const char *x);
int sl_contains(const StrList *s, const char *x);
void sl_clear(StrList *s);
char *sl_join(const StrList *s, char sep);
uint8_t rd8(const uint8_t *b, size_t n, size_t o);
uint16_t rd16(const uint8_t *b, size_t n, size_t o);
uint32_t rd32(const uint8_t *b, size_t n, size_t o);
void wr16(uint8_t *b, size_t n, size_t o, uint32_t v);
void wr32(uint8_t *b, size_t n, size_t o, uint32_t v);
int has_at(const uint8_t *b, size_t n, size_t o, const char *ascii);
long find_bytes(const uint8_t *h, size_t hn, const uint8_t *nd, size_t nn, size_t from);
uint32_t uleb_read(const uint8_t *b, size_t n, size_t *pos);
char *hex_of(const uint8_t *b, size_t n);
void rc4_crypt(const uint8_t *key, size_t klen, const uint8_t *in, uint8_t *out, size_t len);
void rc4_stream(const uint8_t *key, size_t klen, uint8_t *out, size_t n);
void sha1_block(Sha1Ctx *c, const uint8_t *p);
void sha1_init(Sha1Ctx *c);
void sha1_update(Sha1Ctx *c, const uint8_t *b, size_t n);
void sha1_final(Sha1Ctx *c, uint8_t out[20]);
void sha1_of(const uint8_t *b, size_t n, uint8_t out[20]);
uint32_t adler32_of(const uint8_t *b, size_t n);
int read_at(FILE *f, uint64_t off, void *buf, size_t n);
int zip_open(const char *path, ZipReader *z);
void zip_close(ZipReader *z);
ZipEntry *zip_find(ZipReader *z, const char *name);
int zip_read(ZipReader *z, ZipEntry *e, Buf *out);
int zip_writer_open(ZipWriter *w, const char *path);
int zip_writer_add(ZipWriter *w, const char *name, const uint8_t *data, size_t len);
int zip_raw_read(ZipReader *z, ZipEntry *e, Buf *out);
int zip_writer_add_raw(ZipWriter *w, const char *name, int method, uint32_t crc,
                       uint64_t csize, uint64_t usize, const uint8_t *data, size_t len);
int zip_writer_close(ZipWriter *w);
char *xasprintf(const char *fmt, ...);
int container_trailer(const uint8_t *d, size_t n);
size_t cut_from(size_t n, size_t from, uint32_t len, size_t *out_len);
int container_blocks(const uint8_t *d, size_t n, CBlock **out, int *count);
void container_blocks_free(CBlock *b, int n);
int dex_valid(const uint8_t *d, size_t off, size_t len);
void dex_resign(uint8_t *d, size_t n);
uint32_t hash_str(const char *s);
void idx_hash_build(DexIdx *x);
int dex_idx_class(DexIdx *x, const char *desc);
int dex_idx_open(DexIdx *x, const uint8_t *d, size_t n);
void dex_idx_free(DexIdx *x);
void dex_walk(DexIdx *x, uint32_t code_off, insn_fn fn, void *ud);
void hit_add(HitSet *hs, int bi, size_t off, const uint8_t *key, int klen,
                    const uint8_t *dex, size_t len);
uint8_t *decrypt_scheme(const uint8_t *k, int klen, const uint8_t *d, size_t off,
                               size_t size, int scheme);
int block_scan(CBlock *blocks, int nb, const uint8_t *keys, int keylen, int nkeys,
               Hit **out, int *nout);
void hits_free(Hit *h, int n);
char *pick_str(const char *s);
void dex_collect_classes(const uint8_t *d, size_t n, StrList *out);
int prefix_match(const StrList *pref, const char *name);
int in_keep(const char *name);
int dex_drop_classes(uint8_t *b, size_t n, const StrList *prefixes, const StrList *also);
char *pkg_of(const char *desc);
int dex_trim(const uint8_t *in, size_t n, const StrList *prefixes, const StrList *also, Buf *out);
void put_utf8(Vec *o, uint32_t cp);
char *utf16le_to_utf8(const uint8_t *p, size_t units);
uint32_t *utf8_to_utf16(const char *s, size_t *units);
int pool_init(StrPool *p, const uint8_t *b, size_t n, size_t pool_pos);
char *decode_at(const uint8_t *b, size_t n, size_t p, int utf8);
char *pool_get(StrPool *p, int idx);
Buf span_at(const uint8_t *b, size_t n, size_t p, int utf8);
Buf encode_string(const char *s, int utf8);
char *axml_read_package(const uint8_t *buf, size_t n);
char *self_check(const uint8_t *buf, size_t n);
void al_add(AttrList *a, const uint8_t *rec);
void al_free(AttrList *a);
void bl_add_own(BufList *l, Buf b);
void bl_free(BufList *l);
int bd_is_stub_class(const char *cur);
char *attr_string(const uint8_t *buf, size_t n, size_t pos, size_t hdr,
                         const char *attr_name, StrPool *pool);
Buf build_pool(Buf *spans, int nspans, BufList *added, uint32_t flags);
int mkdir_p(const char *path);
int write_file(const char *path, const uint8_t *data, size_t len);
int ends_with_ci(const char *s, const char *suf);
int is_dex_name(const char *n);
uint32_t shash(const char *s, int cap);
void ss_init(SSet *s);
void ss_grow(SSet *s);
void ss_add(SSet *s, const char *x);
void ss_free(SSet *s);
void package_key(const char *pkg, uint8_t key[16]);
Buf deflate_buf(const uint8_t *d, size_t n);
void *dex_job_run(void *arg);
void *dex_pool_run(void *arg);
int is_shell_entry(const char *name);
int zip_copy_raw(ZipReader *z, ZipEntry *e, ZipWriter *w);
uint8_t bd_mul(uint8_t a, uint8_t b);
void bd_sbox_build(void);
void bd_expand(const uint8_t key[16], uint8_t rk[176]);
void bd_enc_block(const uint8_t in[16], const uint8_t rk[176], uint8_t out[16]);
void bd_ctr(uint8_t *d, size_t n, const uint8_t key[16], const uint8_t nonce[12], size_t lim);
uint32_t bd_ror(uint32_t x, int r);
void bd_sha_block(BdSha *c, const uint8_t *p);
void bd_sha_init(BdSha *c);
void bd_sha_update(BdSha *c, const uint8_t *p, size_t n);
void bd_sha_final(BdSha *c, uint8_t out[32]);
void bd_hmac(const uint8_t *key, size_t klen, const uint8_t *msg, size_t mlen, uint8_t out[32]);
int bd_hit_name(const char *s);
void bd_kdf(const uint8_t *k1, size_t k1len, const uint8_t *msg, size_t msglen,
                   const char *mode, uint8_t key[16], uint8_t nonce[12]);
int bd_is_lhex(int c);
void bd_hex_add(BdHexList *l, const uint8_t *p, uint32_t pos);
void bd_hex_scan(const uint8_t *b, size_t n, BdHexList *l);
int bd_match(const uint8_t *so, size_t son, BdHexList *lx, StrList *mds, ZipReader *z,
                    char alg[17], char mode[17]);
uint64_t bd_rd64(const uint8_t *p);
int bd_printable_ratio(const uint8_t *p, size_t n);
int bd_cmp_cand(const void *a, const void *b);
int bd_cmp_win(const void *a, const void *b);
void bd_rot_key(uint64_t k, int s, uint8_t out[8]);
int bd_cmp_win2(const void *a, const void *b);
int bd_xor_crack(Buf *blobs, int nb, StrList *mds, ZipReader *z, char alg[17], char mode[17]);
char *bd_dex_appname(const uint8_t *b, size_t n);
char *bd_dotted(const char *s);
int bd_dinflate(const uint8_t *d, size_t n, Buf *out);
int bd_cmp_ord(const void *a, const void *b);
int baidu_try(const char *apkPath, const char *outDir, UnpackResult *r);
int ep_drop_hit(const char *name);
void ep_init(void);
void ep_dec_block(const uint8_t in[16], const uint8_t rk[176], uint8_t out[16]);
int ep_cbc_dec(const uint8_t *ct, size_t n, const uint8_t key[16],
                      const uint8_t iv[16], Buf *out);
void ep_sha_parts(const uint8_t *a, size_t an, const uint8_t *b, size_t bn,
                         const uint8_t *c, size_t cn, const uint8_t *d, size_t dn,
                         uint8_t out[32]);
void ep_mat(const uint8_t key[16], const uint8_t nonce[16], const char *label,
                   uint8_t out[16]);
void ep_stream(uint8_t *data, size_t n, const uint8_t key[16],
                      const uint8_t nonce[16], const char *label);
int ep_inflate_w(const uint8_t *d, size_t n, int wbits, Buf *out);
uint32_t ep_uleb(const uint8_t *b, size_t n, size_t *p);
int ep_dex_ok(const uint8_t *d, size_t n);
int ep_inflate_dex(const uint8_t *d, size_t n, Buf *out);
void ep_mm_init(EpMethodMap *m, size_t cap);
void ep_mm_put(EpMethodMap *m, uint32_t k, uint32_t v);
int ep_mm_get(EpMethodMap *m, uint32_t k, uint32_t *v);
int ep_dex_methods(const uint8_t *d, size_t n, EpMethodMap *m);
int ep_tail(const uint8_t *outer, size_t n, EpTail *t);
int ep_payload_dexes(const uint8_t *payload, size_t pn, const char *workDir,
                            EpDex **out, int *n);
int ep_key_ok(const uint8_t k[16], const uint8_t *nonce, const uint8_t *ct,
                     const uint8_t *tag, const uint8_t t4[4], size_t clen);
int ep_ent_order(ZipReader *z, int pass, int i);
int ep_find_key(ZipReader *z, const uint8_t *cls, size_t cln, uint8_t key[16],
                       Buf *payload, int *havePayload);
int ep_epc2(const uint8_t *blob, size_t n, const uint8_t key[16], Buf *cfg,
                   EpBlock **blocks, int *nblocks);
uint32_t ep_mix(uint32_t w0, uint32_t w1, uint32_t w2);
int ep_lcg_find(const uint8_t *lib, size_t n, EpLcg **out, int *nout);
void ep_lcg_dec(uint8_t *d, size_t n, uint32_t seed, uint32_t mult, uint32_t inc);
char *ep_type_name(const uint8_t *d, size_t n, uint32_t ti);
uint32_t ep_hash(const char *s);
void ep_sup_init(EpSupMap *m, size_t cap);
void ep_sup_put(EpSupMap *m, const char *k, const char *v);
const char *ep_sup_get(EpSupMap *m, const char *k);
void ep_sup_free(EpSupMap *m);
void ep_build_sup(EpSupMap *m, Buf *dexes, int nd);
int ep_chain(EpSupMap *m, const char *desc, const char *want);
int ep_is_fqcn(const char *s);
char *ep_type_name_string(const uint8_t *d, size_t n, uint32_t si);
void ep_stub_strings(const uint8_t *d, size_t n, StrList *out);
int ep_cmp_ent(const void *a, const void *b);
int ep_cmp_str(const void *a, const void *b);
int ep_cmp_bufsize(const void *a, const void *b);
int ep_json_int(const char *js, const char *key, uint32_t *out);
char *ep_json_str(const char *js, const char *key);
int ep_dex_ord(const char *nm);
int ep_cmp_dex(const void *a, const void *b);
void ep_cleanup(StrList *a, StrList *b, StrList *c);
void ep_entry_factory(EpSupMap *sup, StrList *cands, const char *pkg,
                             char **entry, char **factory);
int ep_cmp_dexsize(const void *a, const void *b);
void sc_apk_base(const char *apkPath, char *out, size_t cap);
void sc_out_dir(char *work, size_t cap, const char *outDir, const char *apkPath,
                       const char *pkg, const char *root, const char *sub);
void ep_make_work(char *work, size_t cap, const char *outDir, const char *apkPath,
                         const char *pkg, const char *variant);
void e2_cfg_free(E2Cfg *c);
const char *e2_skip_ws(const char *p);
const char *e2_find_key(const char *j, const char *key);
char *e2_unescape(const char *s, const char *end);
char *e2_jstr(const char *j, const char *key);
char *e2_jraw(const char *j, const char *key);
int e2_jint(const char *j, const char *key, int dflt, int *has);
void e2_jmap(const char *j, const char *key, StrList *ks, StrList *vs);
int e2_xor_key(const char *raw);
int e2_b64(const char *s, uint8_t **out, size_t *outlen);
char *e2_decode_ep(const char *val, int method, const char *xork, const char *rc4k);
Buf e2_dec_buf(const uint8_t *d, size_t n, int method, const char *xork, const char *rc4k);
uint64_t e2_rd64(const uint8_t *d, size_t n, size_t o);
Buf e2_elf_section(const uint8_t *d, size_t n, const char *want);
char *e2_extract_json(const char *text);
int e2_cfg_parse(const char *j, E2Cfg *c);
Buf e2_arsc_repair(const uint8_t *b, size_t n, int method, const char *xork, const char *rc4k);
int e2_ends(const char *s, const char *suf);
int e2_starts(const char *s, const char *pre);
int e2_in(const StrList *l, const char *s);
int e2_cmp_size(const void *a, const void *b);
int e2_is_shell_dex(const uint8_t *d, size_t n);
int epic2_try(const char *apkPath, const char *outDir, UnpackResult *r);
int epic_try(const char *apkPath, const char *outDir, UnpackResult *r);
void dp_opw_build(void);
int dp_insn_kind(int op);
uint32_t dp_str_off(const uint8_t *d, size_t n, uint32_t si);
char *dp_dex_str(const uint8_t *d, size_t n, uint32_t si);
char *dp_type_name(const uint8_t *d, size_t n, uint32_t ti);
int dp_method_ref(const uint8_t *d, size_t n, uint32_t mi, char **cls, char **nm);
int dp_field_ref(const uint8_t *d, size_t n, uint32_t fi, char **cls);
int dp_walk_methods(const uint8_t *d, size_t n, DpMethodFn fn, void *ctx);
int dp_uleb_len(uint32_t v);
void dp_write_uleb(uint8_t *b, size_t *o, uint32_t v);
uint64_t dp_rd64(const uint8_t *d, size_t n, size_t o);
const uint8_t *dp_memmem(const uint8_t *h, size_t hn, const char *nd, size_t nn);
uint8_t dp_gmul(uint8_t a, uint8_t b);
uint8_t dp_gpow(uint8_t a, int e);
void dp_sb_init(void);
void dp_aes_expand(const uint8_t *key, int klen, uint8_t *rk, int *rnr);
void dp_aes_dec_block(const uint8_t in[16], uint8_t out[16], const uint8_t *rk, int nr);
int dp_aes_cbc_dec(const uint8_t *key, int klen, const uint8_t *iv,
                          const uint8_t *in, size_t n, uint8_t *out);
int dp_elf_sym(const uint8_t *d, size_t n, const char *want, uint8_t out[16]);
int dp_ak_candidates(const uint8_t *d, size_t n, uint8_t out[8][16], int *nout);
uint64_t dp_mix64(uint64_t k);
uint64_t dp_ay_gen_key(int line);
int dp_is_hexc(int c);
void dp_xor_strings(const uint8_t *b, size_t n, const uint8_t k8[8],
                           char out[][33], int *nout, int max);
int dp_ay_scan_key(const uint8_t *d, size_t n, uint64_t k,
                          char out[][33], int *nout, int max);
void *dp_key_job(void *p);
int dp_ay_anchor_keys(const uint8_t *d, size_t n, char out[][33], int max);
int dp_ay_build_keys(const uint8_t *d, size_t n, char out[][33], int max);
int dp_try_cfg(const uint8_t *ak, const uint8_t *cf, size_t cn, const char *km,
                      char **out, uint8_t key[32], int *klen);
int dp_recover_cfg(const uint8_t *so, size_t son, const uint8_t *cf, size_t cn,
                          const char *pkg, const char *verm, uint8_t aks[8][16], int nak,
                          char **out, uint8_t key[32], int *klen);
char *dp_json_str(const char *j, const char *key);
int dp_json_int(const char *j, const char *key, long *out);
int dp_parse_pool(const uint8_t *d, size_t n, int *ver, DpPoolDex **out, int *ocnt);
void dp_pool_free(DpPoolDex *a, int n);
uint32_t dp_uleb(const uint8_t *d, size_t n, size_t *off);
int dp_dex_has_class(const uint8_t *d, size_t n, const char *dotcls);
char *dp_axml_version(const uint8_t *buf, size_t n);
int dp_collect_codes(const uint8_t *d, size_t n, DpCode **out);
double dp_match_score(DpInsn *ins, int nins, DpCode *codes, int nc);
int dp_return_op(int op);
double dp_insn_score(DpInsn *ins, int nins, int mode, const uint8_t *aeskey, int klen);
void dpt_ctx_free(DptCtx *c);
int dpt_load(const char *apkPath, DptCtx *c);
int dpt_gather(DptCtx *c, const char *outDir);
int dpt_try(const char *apkPath, const char *outDir, UnpackResult *r);
int sc_unpack_run(const char *apkPath, const char *outDir, UnpackResult *r);
void sc_unpack_result_free(UnpackResult *r);
JNIEXPORT jobjectArray JNICALL Java_com_mint_toolbox_Native_run(JNIEnv *env, jclass cls,
                                                                 jstring apk, jstring out);

#endif
