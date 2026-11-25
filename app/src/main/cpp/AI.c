int zip_open(const char *path, ZipReader *z)
{
    memset(z, 0, sizeof(*z));
    z->f = fopen(path, "rb");
    if (z->f) setvbuf(z->f, NULL, _IOFBF, 1u << 20);
    if (!z->f) return -1;
    if (fseek(z->f, 0, SEEK_END) != 0) { fclose(z->f); z->f = NULL; return -1; }
    off_t end = ftell(z->f);
    if (end < 22) { fclose(z->f); z->f = NULL; return -1; }
    z->size = (uint64_t) end;

    size_t tail = (size_t) (end < 66560 ? end : 66560);
    uint8_t *b = xmalloc(tail);
    if (read_at(z->f, z->size - tail, b, tail) != 0) { free(b); fclose(z->f); z->f = NULL; return -1; }

    long eocd = -1;
    for (long i = (long) tail - 22; i >= 0; i--) {
        if (b[i] == 'P' && b[i + 1] == 'K' && b[i + 2] == 5 && b[i + 3] == 6) { eocd = i; break; }
    }
    if (eocd < 0) { free(b); fclose(z->f); z->f = NULL; return -1; }

    uint32_t count = (uint32_t) b[eocd + 10] | ((uint32_t) b[eocd + 11] << 8);
    uint64_t cd_size = (uint32_t) b[eocd + 12] | ((uint32_t) b[eocd + 13] << 8)
                     | ((uint32_t) b[eocd + 14] << 16) | ((uint32_t) b[eocd + 15] << 24);
    uint64_t cd_off = (uint32_t) b[eocd + 16] | ((uint32_t) b[eocd + 17] << 8)
                    | ((uint32_t) b[eocd + 18] << 16) | ((uint32_t) b[eocd + 19] << 24);

    
    if (count == 0xffff || cd_off == 0xffffffffu || cd_size == 0xffffffffu) {
        long loc = -1;
        for (long i = eocd - 20; i >= 0; i--) {
            if (b[i] == 'P' && b[i + 1] == 'K' && b[i + 2] == 6 && b[i + 3] == 7) { loc = i; break; }
        }
        if (loc >= 0) {
            uint64_t z64 = (uint64_t) b[loc + 8] | ((uint64_t) b[loc + 9] << 8)
                         | ((uint64_t) b[loc + 10] << 16) | ((uint64_t) b[loc + 11] << 24)
                         | ((uint64_t) b[loc + 12] << 32) | ((uint64_t) b[loc + 13] << 40)
                         | ((uint64_t) b[loc + 14] << 48) | ((uint64_t) b[loc + 15] << 56);
            uint8_t hdr[56];
            if (read_at(z->f, z64, hdr, sizeof(hdr)) == 0
                    && hdr[0] == 'P' && hdr[1] == 'K' && hdr[2] == 6 && hdr[3] == 6) {
                count = (uint32_t) hdr[32] | ((uint32_t) hdr[33] << 8)
                      | ((uint32_t) hdr[34] << 16) | ((uint32_t) hdr[35] << 24);
                cd_size = (uint64_t) hdr[40] | ((uint64_t) hdr[41] << 8) | ((uint64_t) hdr[42] << 16)
                        | ((uint64_t) hdr[43] << 24) | ((uint64_t) hdr[44] << 32)
                        | ((uint64_t) hdr[45] << 40) | ((uint64_t) hdr[46] << 48)
                        | ((uint64_t) hdr[47] << 56);
                cd_off = (uint64_t) hdr[48] | ((uint64_t) hdr[49] << 8) | ((uint64_t) hdr[50] << 16)
                       | ((uint64_t) hdr[51] << 24) | ((uint64_t) hdr[52] << 32)
                       | ((uint64_t) hdr[53] << 40) | ((uint64_t) hdr[54] << 48)
                       | ((uint64_t) hdr[55] << 56);
            }
        }
    }
    free(b);

    if (count == 0 || cd_off + cd_size > z->size) { fclose(z->f); z->f = NULL; return -1; }

    uint8_t *cd = xmalloc((size_t) cd_size);
    if (read_at(z->f, cd_off, cd, (size_t) cd_size) != 0) {
        free(cd); fclose(z->f); z->f = NULL; return -1;
    }

    z->e = xcalloc(count, sizeof(ZipEntry));
    size_t p = 0;
    int n = 0;
    for (uint32_t i = 0; i < count; i++) {
        if (p + 46 > cd_size) break;
        if (!(cd[p] == 'P' && cd[p + 1] == 'K' && cd[p + 2] == 1 && cd[p + 3] == 2)) break;
        uint16_t nlen = (uint16_t) (cd[p + 28] | (cd[p + 29] << 8));
        uint16_t elen = (uint16_t) (cd[p + 30] | (cd[p + 31] << 8));
        uint16_t clen = (uint16_t) (cd[p + 32] | (cd[p + 33] << 8));
        if (p + 46 + nlen + elen + clen > cd_size) break;

        ZipEntry *e = &z->e[n];
        e->method = cd[p + 10] | (cd[p + 11] << 8);
        e->crc = (uint32_t) cd[p + 16] | ((uint32_t) cd[p + 17] << 8)
               | ((uint32_t) cd[p + 18] << 16) | ((uint32_t) cd[p + 19] << 24);
        e->csize = (uint32_t) cd[p + 20] | ((uint32_t) cd[p + 21] << 8)
                 | ((uint32_t) cd[p + 22] << 16) | ((uint32_t) cd[p + 23] << 24);
        e->usize = (uint32_t) cd[p + 24] | ((uint32_t) cd[p + 25] << 8)
                 | ((uint32_t) cd[p + 26] << 16) | ((uint32_t) cd[p + 27] << 24);
        e->lho = (uint32_t) cd[p + 42] | ((uint32_t) cd[p + 43] << 8)
               | ((uint32_t) cd[p + 44] << 16) | ((uint32_t) cd[p + 45] << 24);
        e->name = xmalloc((size_t) nlen + 1);
        memcpy(e->name, cd + p + 46, nlen);
        e->name[nlen] = 0;

        
        size_t ep = p + 46 + nlen;
        size_t ee = ep + elen;
        while (ep + 4 <= ee) {
            uint16_t id = (uint16_t) (cd[ep] | (cd[ep + 1] << 8));
            uint16_t sz = (uint16_t) (cd[ep + 2] | (cd[ep + 3] << 8));
            if (ep + 4 + sz > ee) break;
            if (id == 0x0001) {
                size_t q = ep + 4, qe = q + sz;
                if (e->usize == 0xffffffffu && q + 8 <= qe) {
                    e->usize = (uint64_t) cd[q] | ((uint64_t) cd[q + 1] << 8) | ((uint64_t) cd[q + 2] << 16)
                             | ((uint64_t) cd[q + 3] << 24) | ((uint64_t) cd[q + 4] << 32)
                             | ((uint64_t) cd[q + 5] << 40) | ((uint64_t) cd[q + 6] << 48)
                             | ((uint64_t) cd[q + 7] << 56);
                    q += 8;
                }
                if (e->csize == 0xffffffffu && q + 8 <= qe) {
                    e->csize = (uint64_t) cd[q] | ((uint64_t) cd[q + 1] << 8) | ((uint64_t) cd[q + 2] << 16)
                             | ((uint64_t) cd[q + 3] << 24) | ((uint64_t) cd[q + 4] << 32)
                             | ((uint64_t) cd[q + 5] << 40) | ((uint64_t) cd[q + 6] << 48)
                             | ((uint64_t) cd[q + 7] << 56);
                    q += 8;
                }
                if (e->lho == 0xffffffffu && q + 8 <= qe) {
                    e->lho = (uint64_t) cd[q] | ((uint64_t) cd[q + 1] << 8) | ((uint64_t) cd[q + 2] << 16)
                           | ((uint64_t) cd[q + 3] << 24) | ((uint64_t) cd[q + 4] << 32)
                           | ((uint64_t) cd[q + 5] << 40) | ((uint64_t) cd[q + 6] << 48)
                           | ((uint64_t) cd[q + 7] << 56);
                }
            }
            ep += 4 + sz;
        }
        n++;
        p += 46 + nlen + elen + clen;
    }
    free(cd);
    z->n = n;
    return 0;
}
