void put_utf8(Vec *o, uint32_t cp)
{
    if (cp < 0x80) {
        vec_put8(o, (int) cp);
    } else if (cp < 0x800) {
        vec_put8(o, (int) (0xC0 | (cp >> 6)));
        vec_put8(o, (int) (0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
        vec_put8(o, (int) (0xE0 | (cp >> 12)));
        vec_put8(o, (int) (0x80 | ((cp >> 6) & 0x3F)));
        vec_put8(o, (int) (0x80 | (cp & 0x3F)));
    } else {
        vec_put8(o, (int) (0xF0 | (cp >> 18)));
        vec_put8(o, (int) (0x80 | ((cp >> 12) & 0x3F)));
        vec_put8(o, (int) (0x80 | ((cp >> 6) & 0x3F)));
        vec_put8(o, (int) (0x80 | (cp & 0x3F)));
    }
}
