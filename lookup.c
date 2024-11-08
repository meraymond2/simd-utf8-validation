#include "lookup.h"
#include <immintrin.h>
#include <stdio.h>

const char zeros[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool lookup_validate(const char *bytes, size_t len) {
    size_t l = len - (len % 16);

    __m128i v0 = _mm_load_si128((const __m128i *) zeros);
//    __m128i v1 = _mm_load_si128((const __m128i *) ones);


    return true;
}
