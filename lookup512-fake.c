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


#define simd512_new(a, b) (Simd512){a, b}
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
#define simd512_shuf(lookup, v) simd512_new(_mm256_shuffle_epi8(lookup.a, v.a), _mm256_shuffle_epi8(lookup.b, v.b))
#define simd512_shr(v, n) simd512_new(_mm256_and_si256(_mm256_srli_epi16(v.a, n), _mm256_set1_epi16(0XF0F)), _mm256_and_si256(_mm256_srli_epi16(v.b, n), _mm256_set1_epi16(0XF0F)))
#define simd512_and(x, y) simd512_new(_mm256_and_si256(x.a, y.a), _mm256_and_si256(x.b, y.b))
#define simd512_testz(xs) _mm256_testz_si256(xs.a, xs.a) && _mm256_testz_si256(xs.b, xs.b)

#define simd512_palingr(xs, ys, n) simd512_new( \
  _mm256_alignr_epi8(xs.a, _mm256_permute2x128_si256(ys.b, xs.a, 0x21), 16 - n), \
  _mm256_alignr_epi8(xs.b, _mm256_permute2x128_si256(xs.a, xs.b, 0x21), 16 - n)  \
)

#define simd512_subsat(xs, ys) simd512_new( _mm256_subs_epu8(xs.a, ys.a), _mm256_subs_epu8(xs.b, ys.b) )
#define simd512_or(xs, ys) simd512_new( _mm256_or_si256(xs.a, ys.a), _mm256_or_si256(xs.b, ys.b) )
#define simd512_xor(xs, ys) simd512_new( _mm256_xor_si256(xs.a, ys.a), _mm256_xor_si256(xs.b, ys.b) )

bool lookup512_fake_validate(const unsigned char *bytes, size_t len) {
    size_t l = len - (len % 64);
    size_t i = 0;
    Simd512 v0 = simd512_set1(0);
    // First byte high nibble
    Simd512 table1 = simd512_set(
            1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98,
            1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98,
            1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98,
            1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 6, 2, 26, 98
    );
    Simd512 table2 = simd512_set(
            227, 227, 235, 227, 227, 227, 227, 227, 227, 227, 227, 195, 131, 131, 135, 183,
            227, 227, 235, 227, 227, 227, 227, 227, 227, 227, 227, 195, 131, 131, 135, 183,
            227, 227, 235, 227, 227, 227, 227, 227, 227, 227, 227, 195, 131, 131, 135, 183,
            227, 227, 235, 227, 227, 227, 227, 227, 227, 227, 227, 195, 131, 131, 135, 183
    );
    Simd512 table3 = simd512_set(
            2, 2, 2, 2, 205, 205, 213, 181, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 205, 205, 213, 181, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 205, 205, 213, 181, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 205, 205, 213, 181, 2, 2, 2, 2, 2, 2, 2, 2
    );

    Simd512 acc = simd512_set1(0);
    Simd512 low_mask = simd512_set1(15);

    while (i < l) {
        Simd512 v1 = simd512_load((bytes + i));
        Simd512 prev1 = simd512_palingr(v1, v0, 1);
        Simd512 byte_1_high = simd512_shuf(table1, simd512_shr(prev1, 4));
        Simd512 byte_1_low = simd512_shuf(table2, simd512_and(prev1, low_mask));
        Simd512 byte_2_high = simd512_shuf(table3, simd512_shr(v1, 4));
        Simd512 classification = simd512_and(simd512_and(byte_1_high, byte_1_low), byte_2_high);
        Simd512 prev2 = simd512_palingr(v1, v0, 2);
        Simd512 prev3 = simd512_palingr(v1, v0, 3);
        Simd512 gte_224 = simd512_subsat(prev2, simd512_set1(96));
        Simd512 gte_240 = simd512_subsat(prev3, simd512_set1(112)); // 0111_0000
        Simd512 combined = simd512_or(gte_224, gte_240);
        Simd512 expected_continuations = simd512_and(combined, simd512_set1(0x80));
        Simd512 final = simd512_xor(expected_continuations, classification);


        acc = simd512_or(acc, final);
        i += 64;
        v0 = v1;
    }


    char buf[64] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    memcpy(buf, (bytes + i), len - l);
    bool last_ok = buf[63] < (const unsigned char) 0xc0;
    bool second_last_ok = buf[62] < (const unsigned char) 0xe0;
    bool third_last_ok = buf[61] < (const unsigned char) 0xf0;
    if (!(last_ok && second_last_ok && third_last_ok)) return false;

    Simd512 v1 = simd512_load(buf);
    Simd512 prev1 = simd512_palingr(v1, v0, 1);
    Simd512 byte_1_high = simd512_shuf(table1, simd512_shr(prev1, 4));
    Simd512 byte_1_low = simd512_shuf(table2, simd512_and(prev1, low_mask));
    Simd512 byte_2_high = simd512_shuf(table3, simd512_shr(v1, 4));
    Simd512 classification = simd512_and(simd512_and(byte_1_high, byte_1_low), byte_2_high);
    Simd512 prev2 = simd512_palingr(v1, v0, 2);
    Simd512 prev3 = simd512_palingr(v1, v0, 3);
    Simd512 gte_224 = simd512_subsat(prev2, simd512_set1(96));
    Simd512 gte_240 = simd512_subsat(prev3, simd512_set1(112)); // 0111_0000
    Simd512 combined = simd512_or(gte_224, gte_240);
    Simd512 expected_continuations = simd512_and(combined, simd512_set1(0x80));
    Simd512 final = simd512_xor(expected_continuations, classification);

    acc = simd512_or(acc, final);

    return simd512_testz(acc);
}
