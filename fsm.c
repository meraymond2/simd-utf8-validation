#include "fsm.h"

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

const char category_table[255] = {
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
        INVALID
};
// todo lookup table for state + category to next-state
