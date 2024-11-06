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

enum Category category_table[255] = {
        // 0000_0000 - 0111_1111, 0-127
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        ascii, ascii, ascii, ascii, ascii, ascii, ascii, ascii,
        // 1000_0000 - 1000_1111, 128 - 143
        continuation_low, continuation_low, continuation_low, continuation_low,
        continuation_low, continuation_low, continuation_low, continuation_low,
        continuation_low, continuation_low, continuation_low, continuation_low,
        continuation_low, continuation_low, continuation_low, continuation_low,
        // 1001_0000 - 1001_1111, 144 - 159
        continuation_mid, continuation_mid, continuation_mid, continuation_mid,
        continuation_mid, continuation_mid, continuation_mid, continuation_mid,
        continuation_mid, continuation_mid, continuation_mid, continuation_mid,
        continuation_mid, continuation_mid, continuation_mid, continuation_mid,
        // 1010_0000 - 1011_1111, 160 - 191
        continuation_high, continuation_high, continuation_high, continuation_high,
        continuation_high, continuation_high, continuation_high, continuation_high,
        continuation_high, continuation_high, continuation_high, continuation_high,
        continuation_high, continuation_high, continuation_high, continuation_high,
        continuation_high, continuation_high, continuation_high, continuation_high,
        continuation_high, continuation_high, continuation_high, continuation_high,
        continuation_high, continuation_high, continuation_high, continuation_high,
        continuation_high, continuation_high, continuation_high, continuation_high,
        // 1100_0000, 1100_0001, 192, 193
        invalid, invalid,
        // 1100_0010 - 1101_1111, 194 - 223
        double_prefix, double_prefix, double_prefix, double_prefix,
        double_prefix, double_prefix, double_prefix, double_prefix,
        double_prefix, double_prefix, double_prefix, double_prefix,
        double_prefix, double_prefix,
        // 1110_0000, 224
        triple_check_overlong,
        // 1110_0001 - 1110_1100, 225 - 236
        triple_prefix, triple_prefix, triple_prefix, triple_prefix,
        triple_prefix, triple_prefix, triple_prefix, triple_prefix,
        triple_prefix, triple_prefix, triple_prefix, triple_prefix,
        // 1110_1101, 237
        triple_check_overlong,
        // 1110_1110, 1110_1111, 238, 239,
        triple_prefix, triple_prefix,
        // 1111_0000, 240
        quad_check_overlong,
        // 1111_0001 - 1111_0011, 241 - 243,
        quad_prefix, quad_prefix, quad_prefix,
        // 1111_0100, 244
        quad_check_too_large,
        // 1111_0101 - 1111_1111, 245 - 255
        invalid
};
// todo lookup table for state + category to next-state
