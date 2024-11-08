#ifndef UTF8_VALIDATION_BRANCHY_H
#define UTF8_VALIDATION_BRANCHY_H

#include <immintrin.h>

void vec_debug(__m128i vec);

__m128i vec_shift_into(__m128i a, __m128i b);

__m128i vec_shr(__m128i v, __m128i n);

__m128i vec_bitand(__m128i v, __m128i n);

u_int8_t vec_reduce_or(__m128i v);

#endif //UTF8_VALIDATION_BRANCHY_H
