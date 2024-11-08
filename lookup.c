#include "lookup.h"
#include <immintrin.h>
#include <stdio.h>

const u_int8_t zeros[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const u_int8_t ints[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const u_int8_t fours[16] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

// First byte high nibble
const u_int8_t table1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98};
// First byte low nibble
const u_int8_t table2[16] = {183, 135, 131, 131, 195, 227, 227, 227, 227, 227, 227, 227, 227, 235, 227, 227};
// Second byte high nibble
const u_int8_t table3[16] = {2, 2, 2, 2, 2, 2, 2, 2, 181, 213, 205, 205, 2, 2, 2, 2};

void debug_vec(__m128i v) {
    u_int8_t buf[16];
    _mm_store_si128(buf, v);
    for (int i = 0; i < 16; i++) {
        printf("%d, ", buf[i]);
    }
    printf("\n");
}

__m128i classify(__m128i input, __m128i previous_input) {
    __m128i prev1 = _mm_alignr_epi8(input, previous_input, 15);
    return prev1;
}

bool lookup_validate(const char *bytes, size_t len) {
    size_t l = len - (len % 16);
    size_t i = 0;
    __m128i v0 = _mm_load_si128((const __m128i *) zeros);


    // debug
    __m128i v1 = _mm_load_si128((const __m128i *) bytes + i);
    classify(v1, v0);
    // \debug


//    while (i < l) {
//        __m128i v1 = _mm_load_si128((const __m128i *) bytes + i);
//        i += 16;
//    }


    return true;
}
