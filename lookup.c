#include "lookup.h"
#include <immintrin.h>
#include <stdio.h>

const u_int8_t zeros[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// First byte high nibble
const u_int8_t table1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98};
// First byte low nibble
const u_int8_t table2[16] = {183, 135, 131, 131, 195, 227, 227, 227, 227, 227, 227, 227, 227, 235, 227, 227};
// Second byte high nibble
const u_int8_t table3[16] = {2, 2, 2, 2, 2, 2, 2, 2, 181, 213, 205, 205, 2, 2, 2, 2};


bool lookup_validate(const char *bytes, size_t len) {
    size_t l = len - (len % 16);

    __m128i v0 = _mm_load_si128((const __m128i *) zeros);
//    __m128i v1 = _mm_load_si128((const __m128i *) ones);


    return true;
}
