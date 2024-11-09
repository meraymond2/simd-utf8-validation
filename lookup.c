#include "lookup.h"
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>

void print_vec(__m128i v) {
    char a[16];
    _mm_store_si128((__m128i *) a, v);
    for (int i = 0; i < 16; i++) {
        printf("%hhd ", a[i]);
    }
    printf("\n");
}

#define shr(v) _mm_and_si128(_mm_srli_epi16(v, 4), high_mask_vec)

bool lookup_validate(const char *bytes, size_t len) {
    size_t l = len - (len % 16);
    size_t i = 0;
    __m128i v0 = _mm_set1_epi8(0);
    __m128i high_mask_vec = _mm_set1_epi16(0XF0F);
    __m128i low_mask_vec = _mm_set1_epi8(15);
    // First byte high nibble
    __m128i table1 = _mm_set_epi8(1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98);
    // First byte low nibble
    __m128i table2 = _mm_set_epi8(183, 135, 131, 131, 195, 227, 227, 227, 227, 227, 227, 227, 227, 235, 227, 227);
    // Second byte high nibble
    __m128i table3 = _mm_set_epi8(2, 2, 2, 2, 2, 2, 2, 2, 181, 213, 205, 205, 2, 2, 2, 2);

    u_int8_t acc = 0;

    while (i < l) {
        __m128i v1 = _mm_load_si128((const __m128i *) &bytes[i]);

        __m128i prev1 = _mm_alignr_epi8(v1, v0, 15);
        __m128i byte_1_high = _mm_shuffle_epi8(table1, shr(prev1));
        __m128i byte_1_low = _mm_shuffle_epi8(table2, _mm_and_si128(prev1, low_mask_vec));
        __m128i byte_2_high = _mm_shuffle_epi8(table3, shr(v1));

        volatile __m128i classification = _mm_and_si128(_mm_and_si128(byte_1_high, byte_1_low), byte_2_high);
        print_vec(classification);
//        acc = vec_reduce_or(classify(prev1, v0));
        i += 16;
        v0 = v1;
    }

    return acc == 0;
}
