#include "lookup.h"
#include "vecs.h"
#include <immintrin.h>
#include <stdio.h>

const u_int8_t zeros[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const u_int8_t fours[16] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
const u_int8_t fs[16] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
                         0x0F};
//
//// First byte high nibble
const u_int8_t table1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98};
//// First byte low nibble
const u_int8_t table2[16] = {183, 135, 131, 131, 195, 227, 227, 227, 227, 227, 227, 227, 227, 235, 227, 227};
//// Second byte high nibble
const u_int8_t table3[16] = {2, 2, 2, 2, 2, 2, 2, 2, 181, 213, 205, 205, 2, 2, 2, 2};

__m128i classify(__m128i input, __m128i previous_input) {
    __m128i prev1 = vec_shift_into(previous_input, input);
    __m128i shift_vec = _mm_load_si128((const __m128i *) fours);
    __m128i shifted = vec_shr(prev1, shift_vec);
    __m128i byte_1_high = _mm_shuffle_epi8(_mm_load_si128((const __m128i *) table1), shifted);
    __m128i masked = vec_bitand(prev1, _mm_load_si128((const __m128i *) fs));
    __m128i byte_1_low = _mm_shuffle_epi8(_mm_load_si128((const __m128i *) table2), masked);
    __m128i shifted2 = vec_shr(input, shift_vec);
    __m128i byte_2_high = _mm_shuffle_epi8(_mm_load_si128((const __m128i *) table3), shifted2);

    return vec_bitand(vec_bitand(byte_1_high, byte_1_low), byte_2_high);
}
//
//bool lookup_validate(const char *bytes, size_t len) {
//    __m128i v0 = _mm_load_si128((const __m128i *) zeros);
//    __m128i v1 = _mm_load_si128((const __m128i *) bytes);
//    vec_debug(classify(v1, v0));
//    return true;
//}

bool lookup_validate(const char *bytes, size_t len) {
    size_t l = len - (len % 16);
    size_t i = 0;
    __m128i v0 = _mm_load_si128((const __m128i *) zeros);

    u_int8_t acc = 0;

    while (i < l) {
//        __m128i v1 = _mm_load_si128((const __m128i *) bytes + i);
        __m128i v1 = _mm_load_si128((const __m128i *) zeros);
        acc = vec_reduce_or(classify(v1, v0));
        i += 16;
        v0 = v1;
    }

    return acc == 0;
}
