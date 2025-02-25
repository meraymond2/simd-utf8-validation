#include <stdio.h>
#include "fsm.h"

/*
 *
 * */
const char VALID = 0;
const char ONE_MORE = 12;
const char TWO_MORE = 24;
const char THREE_MORE = 36;
const char CHECK_3_BYTE_OVERLONG = 48;
const char CHECK_3_BYTE_SURROGATE = 60;
const char CHECK_4_BYTE_OVERLONG = 72;
const char CHECK_4_BYTE_TOO_LARGE = 84;
const char ERROR = 96; // could split into types

/*
Bytes are classified as belonging to one of twelve possible categories.
  1. ascii, 0000_0000 - 0111_1111
  2. double-prefix, 1100_0010 - 1101_1111
  3. triple-prefix, 1110_0001 - 1110_1100 or 1110_1110 - 1110_1111
  4. quad-prefix, 1111_0001 - 1111_0011
  5. triple-check-overlong, 1110_0000
  6. triple-check-surrogate, 1110_1101
  7. quad-check-overlong, 1111_0000
  8. quad-check-too-large, 1111_0100
  9. error, 1100_0000 - 1100_0001 or 1111_0101 - 1111_1111
  10. continuation-low, 1000_0000 - 1000_1111
  11. continuation-mid, 1001_0000 - 1000_1111
  12. continuation-high, 1010_0000 - 1011_1111
 */

const char ASCII = 0;
const char DOUBLE_PREFIX = 1;
const char TRIPLE_PREFIX = 2;
const char QUAD_PREFIX = 3;
const char TRIPLE_CHECK_OVERLONG = 4;
const char TRIPLE_CHECK_SURROGATE = 5;
const char QUAD_CHECK_OVERLONG = 6;
const char QUAD_CHECK_TOO_LARGE = 7;
const char CONTINUATION_LOW = 8;
const char CONTINUATION_MID = 9;
const char CONTINUATION_HIGH = 10;
const char INVALID = 11;

const char category_table[256] = {
        // 0000_0000 - 0111_1111, 0-127
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII, ASCII,
        // 1000_0000 - 1000_1111, 128 - 143
        CONTINUATION_LOW, CONTINUATION_LOW, CONTINUATION_LOW, CONTINUATION_LOW,
        CONTINUATION_LOW, CONTINUATION_LOW, CONTINUATION_LOW, CONTINUATION_LOW,
        CONTINUATION_LOW, CONTINUATION_LOW, CONTINUATION_LOW, CONTINUATION_LOW,
        CONTINUATION_LOW, CONTINUATION_LOW, CONTINUATION_LOW, CONTINUATION_LOW,
        // 1001_0000 - 1001_1111, 144 - 159
        CONTINUATION_MID, CONTINUATION_MID, CONTINUATION_MID, CONTINUATION_MID,
        CONTINUATION_MID, CONTINUATION_MID, CONTINUATION_MID, CONTINUATION_MID,
        CONTINUATION_MID, CONTINUATION_MID, CONTINUATION_MID, CONTINUATION_MID,
        CONTINUATION_MID, CONTINUATION_MID, CONTINUATION_MID, CONTINUATION_MID,
        // 1010_0000 - 1011_1111, 160 - 191
        CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH,
        CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH,
        CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH,
        CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH,
        CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH,
        CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH,
        CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH,
        CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH, CONTINUATION_HIGH,
        // 1100_0000, 1100_0001, 192, 193
        INVALID, INVALID,
        // 1100_0010 - 1101_1111, 194 - 223
        DOUBLE_PREFIX, DOUBLE_PREFIX, DOUBLE_PREFIX, DOUBLE_PREFIX,
        DOUBLE_PREFIX, DOUBLE_PREFIX, DOUBLE_PREFIX, DOUBLE_PREFIX,
        DOUBLE_PREFIX, DOUBLE_PREFIX, DOUBLE_PREFIX, DOUBLE_PREFIX,
        DOUBLE_PREFIX, DOUBLE_PREFIX,
        // 1110_0000, 224
        TRIPLE_CHECK_OVERLONG,
        // 1110_0001 - 1110_1100, 225 - 236
        TRIPLE_PREFIX, TRIPLE_PREFIX, TRIPLE_PREFIX, TRIPLE_PREFIX,
        TRIPLE_PREFIX, TRIPLE_PREFIX, TRIPLE_PREFIX, TRIPLE_PREFIX,
        TRIPLE_PREFIX, TRIPLE_PREFIX, TRIPLE_PREFIX, TRIPLE_PREFIX,
        // 1110_1101, 237
        TRIPLE_CHECK_SURROGATE,
        // 1110_1110, 1110_1111, 238, 239,
        TRIPLE_PREFIX, TRIPLE_PREFIX,
        // 1111_0000, 240
        QUAD_CHECK_OVERLONG,
        // 1111_0001 - 1111_0011, 241 - 243,
        QUAD_PREFIX, QUAD_PREFIX, QUAD_PREFIX,
        // 1111_0100, 244
        QUAD_CHECK_TOO_LARGE,
        // 1111_0101 - 1111_1111, 245 - 255
        INVALID, INVALID, INVALID, INVALID,
        INVALID, INVALID, INVALID, INVALID,
        INVALID, INVALID
};

const char next_state[108] = {
        // VALID + ASCII
        VALID,
        // VALID + DOUBLE_PREFIX
        ONE_MORE,
        // VALID + TRIPLE_PREFIX
        TWO_MORE,
        // VALID + QUAD_PREFIX
        THREE_MORE,
        // VALID + TRIPLE_CHECK_OVERLONG
        CHECK_3_BYTE_OVERLONG,
        // VALID + TRIPLE_CHECK_SURROGATE
        CHECK_3_BYTE_SURROGATE,
        // VALID + QUAD_CHECK_OVERLONG
        CHECK_4_BYTE_OVERLONG,
        // VALID + QUAD_CHECK_TOO_LARGE
        CHECK_4_BYTE_TOO_LARGE,
        // VALID + CONTINUATION_LOW
        ERROR,
        // VALID + CONTINUATION_MID
        ERROR,
        // VALID + CONTINUATION_HIGH
        ERROR,
        // VALID + INVALID
        ERROR,
        // ONE_MORE + ASCII
        ERROR,
        // ONE_MORE + DOUBLE_PREFIX
        ERROR,
        // ONE_MORE + TRIPLE_PREFIX
        ERROR,
        // ONE_MORE + QUAD_PREFIX
        ERROR,
        // ONE_MORE + TRIPLE_CHECK_OVERLONG
        ERROR,
        // ONE_MORE + TRIPLE_CHECK_SURROGATE
        ERROR,
        // ONE_MORE + QUAD_CHECK_OVERLONG
        ERROR,
        // ONE_MORE + QUAD_CHECK_TOO_LARGE
        ERROR,
        // ONE_MORE + CONTINUATION_LOW
        VALID,
        // ONE_MORE + CONTINUATION_MID
        VALID,
        // ONE_MORE + CONTINUATION_HIGH
        VALID,
        // ONE_MORE + INVALID
        ERROR,
        // TWO_MORE + ASCII
        ERROR,
        // TWO_MORE + DOUBLE_PREFIX
        ERROR,
        // TWO_MORE + TRIPLE_PREFIX
        ERROR,
        // TWO_MORE + QUAD_PREFIX
        ERROR,
        // TWO_MORE + TRIPLE_CHECK_OVERLONG
        ERROR,
        // TWO_MORE + TRIPLE_CHECK_SURROGATE
        ERROR,
        // TWO_MORE + QUAD_CHECK_OVERLONG
        ERROR,
        // TWO_MORE + QUAD_CHECK_TOO_LARGE
        ERROR,
        // TWO_MORE + CONTINUATION_LOW
        ONE_MORE,
        // TWO_MORE + CONTINUATION_MID
        ONE_MORE,
        // TWO_MORE + CONTINUATION_HIGH
        ONE_MORE,
        // TWO_MORE + INVALID
        ERROR,
        // THREE_MORE + ASCII
        ERROR,
        // THREE_MORE + DOUBLE_PREFIX
        ERROR,
        // THREE_MORE + TRIPLE_PREFIX
        ERROR,
        // THREE_MORE + QUAD_PREFIX
        ERROR,
        // THREE_MORE + TRIPLE_CHECK_OVERLONG
        ERROR,
        // THREE_MORE + TRIPLE_CHECK_SURROGATE
        ERROR,
        // THREE_MORE + QUAD_CHECK_OVERLONG
        ERROR,
        // THREE_MORE + QUAD_CHECK_TOO_LARGE
        ERROR,
        // THREE_MORE + CONTINUATION_LOW
        TWO_MORE,
        // THREE_MORE + CONTINUATION_MID
        TWO_MORE,
        // THREE_MORE + CONTINUATION_HIGH
        TWO_MORE,
        // THREE_MORE + INVALID
        ERROR,
        // CHECK_3_BYTE_OVERLONG + ASCII
        ERROR,
        // CHECK_3_BYTE_OVERLONG + DOUBLE_PREFIX
        ERROR,
        // CHECK_3_BYTE_OVERLONG + TRIPLE_PREFIX
        ERROR,
        // CHECK_3_BYTE_OVERLONG + QUAD_PREFIX
        ERROR,
        // CHECK_3_BYTE_OVERLONG + TRIPLE_CHECK_OVERLONG
        ERROR,
        // CHECK_3_BYTE_OVERLONG + TRIPLE_CHECK_SURROGATE
        ERROR,
        // CHECK_3_BYTE_OVERLONG + QUAD_CHECK_OVERLONG
        ERROR,
        // CHECK_3_BYTE_OVERLONG + QUAD_CHECK_TOO_LARGE
        ERROR,
        // CHECK_3_BYTE_OVERLONG + CONTINUATION_LOW
        ERROR,
        // CHECK_3_BYTE_OVERLONG + CONTINUATION_MID
        ERROR,
        // CHECK_3_BYTE_OVERLONG + CONTINUATION_HIGH
        ONE_MORE,
        // CHECK_3_BYTE_OVERLONG + INVALID
        ERROR,
        // CHECK_3_BYTE_SURROGATE + ASCII
        ERROR,
        // CHECK_3_BYTE_SURROGATE + DOUBLE_PREFIX
        ERROR,
        // CHECK_3_BYTE_SURROGATE + TRIPLE_PREFIX
        ERROR,
        // CHECK_3_BYTE_SURROGATE + QUAD_PREFIX
        ERROR,
        // CHECK_3_BYTE_SURROGATE + TRIPLE_CHECK_OVERLONG
        ERROR,
        // CHECK_3_BYTE_SURROGATE + TRIPLE_CHECK_SURROGATE
        ERROR,
        // CHECK_3_BYTE_SURROGATE + QUAD_CHECK_OVERLONG
        ERROR,
        // CHECK_3_BYTE_SURROGATE + QUAD_CHECK_TOO_LARGE
        ERROR,
        // CHECK_3_BYTE_SURROGATE + CONTINUATION_LOW
        ONE_MORE,
        // CHECK_3_BYTE_SURROGATE + CONTINUATION_MID
        ONE_MORE,
        // CHECK_3_BYTE_SURROGATE + CONTINUATION_HIGH
        ERROR,
        // CHECK_3_BYTE_SURROGATE + INVALID
        ERROR,
        // CHECK_4_BYTE_OVERLONG + ASCII
        ERROR,
        // CHECK_4_BYTE_OVERLONG + DOUBLE_PREFIX
        ERROR,
        // CHECK_4_BYTE_OVERLONG + TRIPLE_PREFIX
        ERROR,
        // CHECK_4_BYTE_OVERLONG + QUAD_PREFIX
        ERROR,
        // CHECK_4_BYTE_OVERLONG + TRIPLE_CHECK_OVERLONG
        ERROR,
        // CHECK_4_BYTE_OVERLONG + TRIPLE_CHECK_SURROGATE
        ERROR,
        // CHECK_4_BYTE_OVERLONG + QUAD_CHECK_OVERLONG
        ERROR,
        // CHECK_4_BYTE_OVERLONG + QUAD_CHECK_TOO_LARGE
        ERROR,
        // CHECK_4_BYTE_OVERLONG + CONTINUATION_LOW
        ERROR,
        // CHECK_4_BYTE_OVERLONG + CONTINUATION_MID
        TWO_MORE,
        // CHECK_4_BYTE_OVERLONG + CONTINUATION_HIGH
        TWO_MORE,
        // CHECK_4_BYTE_OVERLONG + INVALID
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + ASCII
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + DOUBLE_PREFIX
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + TRIPLE_PREFIX
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + QUAD_PREFIX
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + TRIPLE_CHECK_OVERLONG
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + TRIPLE_CHECK_SURROGATE
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + QUAD_CHECK_OVERLONG
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + QUAD_CHECK_TOO_LARGE
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + CONTINUATION_LOW
        TWO_MORE,
        // CHECK_4_BYTE_TOO_LARGE + CONTINUATION_MID
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + CONTINUATION_HIGH
        ERROR,
        // CHECK_4_BYTE_TOO_LARGE + INVALID
        ERROR,
        // ERROR + ASCII
        ERROR,
        // ERROR + DOUBLE_PREFIX
        ERROR,
        // ERROR + TRIPLE_PREFIX
        ERROR,
        // ERROR + QUAD_PREFIX
        ERROR,
        // ERROR + TRIPLE_CHECK_OVERLONG
        ERROR,
        // ERROR + TRIPLE_CHECK_SURROGATE
        ERROR,
        // ERROR + QUAD_CHECK_OVERLONG
        ERROR,
        // ERROR + QUAD_CHECK_TOO_LARGE
        ERROR,
        // ERROR + CONTINUATION_LOW
        ERROR,
        // ERROR + CONTINUATION_MID
        ERROR,
        // ERROR + CONTINUATION_HIGH
        ERROR,
        // ERROR + INVALID
        ERROR,
};

size_t index_of_leading_byte(const unsigned char *bytes, size_t len, size_t start) {
    size_t i = start;
    while (i < len) {
        unsigned char b = bytes[i];
        unsigned char category = category_table[b];
        if (category == ASCII || category == DOUBLE_PREFIX || category == TRIPLE_PREFIX || category == QUAD_PREFIX) {
            return i;
        }
        i++;
    }
    return i;
}

bool fsm_validate(const unsigned char *bytes, size_t len) {
    if (len < 32) {
        unsigned char state = VALID;
        size_t i = 0;
        while (i < len) {
            unsigned char b = bytes[i];
            unsigned char category = category_table[b];
            unsigned char idx = category + state;
            state = next_state[idx];
            i++;
        }
        return state == VALID;
    }

    size_t chunk_size = len / 3;
    size_t i = 0;
    size_t j = index_of_leading_byte(bytes, len, chunk_size);
    size_t k = index_of_leading_byte(bytes, len, chunk_size * 2);

    unsigned char s1 = VALID;
    unsigned char s2 = VALID;
    unsigned char s3 = VALID;
    while (i < j && j < k && k < len) {
        unsigned char b1 = bytes[i];
        unsigned char b2 = bytes[j];
        unsigned char b3 = bytes[k];
        unsigned char c1 = category_table[b1];
        unsigned char c2 = category_table[b2];
        unsigned char c3 = category_table[b3];
        s1 = next_state[c1 + s1];
        s2 = next_state[c2 + s2];
        s3 = next_state[c3 + s3];
        i++;
        j++;
        k++;
    }
    return s1 == VALID && s2 == VALID && s3 == VALID;
}
