/* remote_screen_rle.c - run-length encoding of screen rectangles. See remote_screen.h. */
#include "remote_screen.h"

#include <string.h>

size_t remote_screen_rle(const uint16_t *px, uint32_t w, uint32_t h, uint8_t *out, size_t n)
{
    size_t total = (size_t)w * h, o = 0, i = 0;
    while (i < total) {
        uint16_t c = px[i];
        size_t run = 1;
        while (i + run < total && px[i + run] == c && run < 0xFFFF) run++;
        if (o + 4 > n) return 0;
        out[o++] = (uint8_t)(run & 0xFF);
        out[o++] = (uint8_t)(run >> 8);
        out[o++] = (uint8_t)(c & 0xFF);
        out[o++] = (uint8_t)(c >> 8);
        i += run;
    }
    return o;
}
