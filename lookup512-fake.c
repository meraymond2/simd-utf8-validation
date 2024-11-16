#include "lookup512-fake.h"
#include <immintrin.h>
#include <stdio.h>
#include <string.h>

typedef struct Simd512 {
    __m256i a;
    __m256i b;
} Simd512;

void print_vec512(Simd512 v) {
    unsigned char a[64];
    _mm256_store_si256((__m256i *) a, v.a);
    for (int i = 0; i < 32; i++) {
        printf("%02X ", a[i]);
    }
    _mm256_store_si256((__m256i *) a, v.b);
    for (int i = 0; i < 32; i++) {
        printf("%02X ", a[i]);
    }
    printf("\n");
}


#define simd512_new(a, b) (Simd512){a, b};
#define simd512_load(bytes) simd512_new(_mm256_loadu_si256((const __m256i *) bytes), _mm256_loadu_si256((const __m256i *) (bytes + 32)))
#define simd512_set1(n) simd512_new(_mm256_set1_epi8(n), _mm256_set1_epi8(n))
#define simd512_set(\
    n63, n62, n61, n60, n59, n58, n57, n56, \
    n55, n54, n53, n52, n51, n50, n49, n48, \
    n47, n46, n45, n44, n43, n42, n41, n40, \
    n39, n38, n37, n36, n35, n34, n33, n32, \
    n31, n30, n29, n28, n27, n26, n25, n24, \
    n23, n22, n21, n20, n19, n18, n17, n16, \
    n15, n14, n13, n12, n11, n10, n9, n8, \
    n7, n6, n5, n4, n3, n2, n1, n0 \
) simd512_new( \
    _mm256_set_epi8( \
        n32, n33, n34, n35, n36, n37, n38, n39, \
        n40, n41, n42, n43, n44, n45, n46, n47, \
        n48, n49, n50, n51, n52, n53, n54, n55, \
        n56, n57, n58, n59, n60, n61, n62, n63 \
    ), \
    _mm256_set_epi8( \
         n0, n1, n2, n3, n4, n5, n6, n7, n8, \
         n9, n10, n11, n12, n13, n14, n15, n16,  \
         n17, n18, n19, n20, n21, n22, n23, n24, \
         n25, n26, n27, n28, n29, n30, n31 \
    ))
//#define shr(v) _mm256_and_si256(_mm256_srli_epi16(v, 4), high_mask_vec)

//bool lookup256_validate(const unsigned char *bytes, size_t len) {
//    size_t l = len - (len % 32);
//    size_t i = 0;
//    __m256i v0 = _mm256_set1_epi8(0);
//    __m256i high_mask_vec = _mm256_set1_epi16(0XF0F);
//    __m256i low_mask_vec = _mm256_set1_epi8(15);
//    // First byte high nibble
//    __m256i table1 = _mm256_set_epi8(
//            98, 26, 2, 6, 128, 128, 128, 128, 1, 1, 1, 1, 1, 1, 1, 1,
//            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//    );
//    // First byte low nibble
//    __m256i table2 = _mm256_set_epi8(
//            227, 227, 235, 227,
//            227, 227, 227, 227,
//            227, 227, 227, 195,
//            131, 131, 135, 183,
//            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
//    // Second byte high nibble
//    __m256i table3 = _mm256_set_epi8(2, 2, 2, 2, 205, 205,
//                                     213, 181, 2, 2, 2, 2,
//                                     2, 2, 2, 2,
//                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
//    __m256i acc = _mm256_set1_epi8(0);
//
//    while (i < l) {
//        // TODO: document this first part
//        __m256i v1 = _mm256_loadu_si256((const __m256i *) &bytes[i]);
//        __m256i prev1 = _mm256_alignr_epi8(v1, v0, 31);
//        __m256i byte_1_high = _mm256_shuffle_epi8(table1, shr(prev1));
//        __m256i byte_1_low = _mm256_shuffle_epi8(table2, _mm256_and_si256(prev1, low_mask_vec));
//        __m256i byte_2_high = _mm256_shuffle_epi8(table3, shr(v1));
//
//        __m256i classification = _mm256_and_si256(_mm256_and_si256(byte_1_high, byte_1_low), byte_2_high);
//        __m256i prev2 = _mm256_alignr_epi8(v1, v0, 30);
//        __m256i prev3 = _mm256_alignr_epi8(v1, v0, 29);
//
//        // For the 3-4 byte check, we first identify everywhere in the chunk that we expect to see continuation bytes.
//        // We only need to look for the 3rd and 4th bytes, because we've already handled the 2nd bytes as part of the
//        // 2-byte checks.
//        // To identify where we _expect_ 3rd/4th continuation bytes, we identify the 3rd/4th-prefix bytes, and then
//        // shift them over.
//        // We start by creating a mask where we expect to find the second continuation bytes, for both 3- and 4-byte
//        // sequences. We shift the input by 2, and then look for the 3- and 4-prefix bytes. We do that by getting all
//        // the bytes >= 1110_0000.
//        // There isn't an instruction for vector gte, so we use saturating subtraction, subtracting 0110_0000 from each
//        // byte. After that, only bytes >= 1110_0000 will have the high bit set, so we can then & them with 0x80 to
//        // identify them. Once we've &ed them with the mask, we end up with a vector of 0s and 0x80s.
//        __m256i gte_224 = _mm256_subs_epu8(prev2, _mm256_set1_epi8(96)); // 0110_0000
//        // We do the same procedure to look for the third continuation bytes. This time, we shift over by 3. And we only
//        // want prefix bytes >= 1111_0000, because these only apply to 4-byte sequences.
//        __m256i gte_240 = _mm256_subs_epu8(prev3, _mm256_set1_epi8(112)); // 0111_0000
//        // We combine them into a single mask.
//        __m256i combined = _mm256_or_si256(gte_224, gte_240);
//        // & them with 0x80, to get just the ones that still have the high bit set, i.e. are >= the desired amount
//        // We're doing ((prev2 - 96) | (prev3 - 112)) & 0x80, which is logically equivalent to
//        // ((prev2 - 96) & 0x80) | ((prev3 - 112) & 0x80), but one instruction shorter (also less clear).
//        __m256i expected_continuations = _mm256_and_si256(combined, _mm256_set1_epi8(0x80));
//        // The previous classification procedure identified all the continuation bytes using the high bit of the
//        // classification byte. So now we xor the classifications with the continuations. Any expected and actual
//        // continuations with xor out to 0, and any extra or missing continuations will become 1. Also, any other errors
//        // will stay as 1.
//        // That leaves us with a vector of bits that should all be 0.
//        __m256i final = _mm256_xor_si256(expected_continuations, classification);
//        // Or any set bits into the acc
//        acc = _mm256_or_si256(acc, final);
//        i += 32;
//        v0 = v1;
//    }
//
//    char buf[32] = {
//            0, 0, 0, 0, 0, 0, 0, 0,
//            0, 0, 0, 0, 0, 0, 0, 0,
//            0, 0, 0, 0, 0, 0, 0, 0,
//            0, 0, 0, 0, 0, 0, 0, 0};
//    memcpy(buf, &bytes[i], len - l);
//    // Check that end of bytes are valid utf-8.
//    bool last_ok = buf[31] < (const unsigned char) 0xc0;
//    bool second_last_ok = buf[30] < (const unsigned char) 0xe0;
//    bool third_last_ok = buf[29] < (const unsigned char) 0xf0;
//    if (!(last_ok && second_last_ok && third_last_ok)) return false;
//
//    __m256i v1 = _mm256_loadu_si256((const __m256i *) &bytes[i]);
//    __m256i prev1 = _mm256_alignr_epi8(v1, v0, 31);
//    __m256i byte_1_high = _mm256_shuffle_epi8(table1, shr(prev1));
//    __m256i byte_1_low = _mm256_shuffle_epi8(table2, _mm256_and_si256(prev1, low_mask_vec));
//    __m256i byte_2_high = _mm256_shuffle_epi8(table3, shr(v1));
//    __m256i classification = _mm256_and_si256(_mm256_and_si256(byte_1_high, byte_1_low), byte_2_high);
//    __m256i prev2 = _mm256_alignr_epi8(v1, v0, 30);
//    __m256i prev3 = _mm256_alignr_epi8(v1, v0, 29);
//    __m256i gte_224 = _mm256_subs_epu8(prev2, _mm256_set1_epi8(96)); // 0110_0000
//    __m256i gte_240 = _mm256_subs_epu8(prev3, _mm256_set1_epi8(112)); // 0111_0000
//    __m256i combined = _mm256_or_si256(gte_224, gte_240);
//    __m256i expected_continuations = _mm256_and_si256(combined, _mm256_set1_epi8(0x80));
//    __m256i final = _mm256_xor_si256(expected_continuations, classification);
//    acc = _mm256_or_si256(acc, final);
//
//    return _mm256_testz_si256(acc, acc);
//}


bool lookup512_fake_validate(const unsigned char *bytes, size_t len) {
    Simd512 v1 = simd512_load(bytes);


    size_t l = len - (len % 64);
    size_t i = 0;
    Simd512 v0 = simd512_set1(0);
    Simd512 low_mask_vec = simd512_set1(15);
    // First byte high nibble
    Simd512 table1 = simd512_set(
            1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    );
//    // First byte low nibble
//    __m256i table2 = _mm256_set_epi8(
//            227, 227, 235, 227,
//            227, 227, 227, 227,
//            227, 227, 227, 195,
//            131, 131, 135, 183,
//            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
//    // Second byte high nibble
//    __m256i table3 = _mm256_set_epi8(2, 2, 2, 2, 205, 205,
//                                     213, 181, 2, 2, 2, 2,
//                                     2, 2, 2, 2,
//                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
//    __m256i acc = _mm256_set1_epi8(0);

    print_vec512(table1);
    return true;
}
