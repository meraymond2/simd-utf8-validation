#include "lookup.h"
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_vec(__m128i v) {
    unsigned char a[16];
    _mm_store_si128((__m128i *) a, v);
    for (int i = 0; i < 16; i++) {
        printf("%02X ", a[i]);
    }
    printf("\n");
}

#define shr(v) _mm_and_si128(_mm_srli_epi16(v, 4), high_mask_vec)

bool lookup_validate(const unsigned char *bytes, size_t len) {
    size_t l = len - (len % 16);
    size_t i = 0;
    __m128i v0 = _mm_set1_epi8(0);
    __m128i high_mask_vec = _mm_set1_epi16(0XF0F);
    __m128i low_mask_vec = _mm_set1_epi8(15);
    // First byte high nibble
    __m128i table1 = _mm_set_epi8(98, 26, 2, 6, 128, 128, 128, 128, 1, 1, 1, 1, 1, 1, 1, 1);
    // First byte low nibble
    __m128i table2 = _mm_set_epi8(227, 227, 235, 227,
                                  227, 227, 227, 227,
                                  227, 227, 227, 195,
                                  131, 131, 135, 183);
    // Second byte high nibble
    __m128i table3 = _mm_set_epi8(2, 2, 2, 2, 205, 205,
                                  213, 181, 2, 2, 2, 2,
                                  2, 2, 2, 2);

    __m128i acc = _mm_set1_epi8(0);

    while (i < l) {
        // TODO: document this first part
        __m128i v1 = _mm_load_si128((const __m128i *) &bytes[i]);
        __m128i prev1 = _mm_alignr_epi8(v1, v0, 15);
        __m128i byte_1_high = _mm_shuffle_epi8(table1, shr(prev1));
        __m128i byte_1_low = _mm_shuffle_epi8(table2, _mm_and_si128(prev1, low_mask_vec));
        __m128i byte_2_high = _mm_shuffle_epi8(table3, shr(v1));

        __m128i classification = _mm_and_si128(_mm_and_si128(byte_1_high, byte_1_low), byte_2_high);
        __m128i prev2 = _mm_alignr_epi8(v1, v0, 14);
        __m128i prev3 = _mm_alignr_epi8(v1, v0, 13);

        // For the 3-4 byte check, we first identify everywhere in the chunk that we expect to see continuation bytes.
        // We only need to look for the 3rd and 4th bytes, because we've already handled the 2nd bytes as part of the
        // 2-byte checks.
        // To identify where we _expect_ 3rd/4th continuation bytes, we identify the 3rd/4th-prefix bytes, and then
        // shift them over.
        // We start by creating a mask where we expect to find the second continuation bytes, for both 3- and 4-byte
        // sequences. We shift the input by 2, and then look for the 3- and 4-prefix bytes. We do that by getting all
        // the bytes >= 1110_0000.
        // There isn't an instruction for vector gte, so we use saturating subtraction, subtracting 0110_0000 from each
        // byte. After that, only bytes >= 1110_0000 will have the high bit set, so we can then & them with 0x80 to
        // identify them. Once we've &ed them with the mask, we end up with a vector of 0s and 0x80s.
        __m128i gte_224 = _mm_subs_epu8(prev2, _mm_set1_epi8(96)); // 0110_0000
        // We do the same procedure to look for the third continuation bytes. This time, we shift over by 3. And we only
        // want prefix bytes >= 1111_0000, because these only apply to 4-byte sequences.
        __m128i gte_240 = _mm_subs_epu8(prev3, _mm_set1_epi8(112)); // 0111_0000
        // We combine them into a single mask.
        __m128i combined = _mm_or_si128(gte_224, gte_240);
        // & them with 0x80, to get just the ones that still have the high bit set, i.e. are >= the desired amount
        // We're doing ((prev2 - 96) | (prev3 - 112)) & 0x80, which is logically equivalent to
        // ((prev2 - 96) & 0x80) | ((prev3 - 112) & 0x80), but one instruction shorter (also less clear).
        __m128i expected_continuations = _mm_and_si128(combined, _mm_set1_epi8(0x80));
        // The previous classification procedure identified all the continuation bytes using the high bit of the
        // classification byte. So now we xor the classifications with the continuations. Any expected and actual
        // continuations with xor out to 0, and any extra or missing continuations will become 1. Also, any other errors
        // will stay as 1.
        // That leaves us with a vector of bits that should all be 0.
        __m128i final = _mm_xor_si128(expected_continuations, classification);
        // Or any set bits into the acc
        acc = _mm_or_si128(acc, final);
        i += 16;
        v0 = v1;
    }


    char buf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    memcpy(buf, &bytes[i], len - l);
    // Check that end of bytes are valid utf-8.
    bool last_ok = buf[15] < (const unsigned char)0xc0;
    bool second_last_ok = buf[14] < (const unsigned char)0xe0;
    bool third_last_ok = buf[13] < (const unsigned char)0xf0;
    if (!(last_ok && second_last_ok && third_last_ok)) return false;

    __m128i v1 = _mm_load_si128((const __m128i *) &buf);
    __m128i prev1 = _mm_alignr_epi8(v1, v0, 15);
    __m128i byte_1_high = _mm_shuffle_epi8(table1, shr(prev1));
    __m128i byte_1_low = _mm_shuffle_epi8(table2, _mm_and_si128(prev1, low_mask_vec));
    __m128i byte_2_high = _mm_shuffle_epi8(table3, shr(v1));

    __m128i classification = _mm_and_si128(_mm_and_si128(byte_1_high, byte_1_low), byte_2_high);
    __m128i prev2 = _mm_alignr_epi8(v1, v0, 14);
    __m128i prev3 = _mm_alignr_epi8(v1, v0, 13);
    __m128i gte_224 = _mm_subs_epu8(prev2, _mm_set1_epi8(96)); // 0110_0000
    __m128i gte_240 = _mm_subs_epu8(prev3, _mm_set1_epi8(112)); // 0111_0000
    __m128i combined = _mm_or_si128(gte_224, gte_240);
    __m128i expected_continuations = _mm_and_si128(combined, _mm_set1_epi8(0x80));
    __m128i final = _mm_xor_si128(expected_continuations, classification);
    acc = _mm_or_si128(acc, final);

    return _mm_testz_si128(acc, acc);
}
