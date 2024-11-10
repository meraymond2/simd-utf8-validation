#include <stdbool.h>
#include <stdlib.h>

bool is_continuation(unsigned char byte) {
    return (byte >> 6) == 2;
}

bool branchy_validate(const unsigned char *bytes, size_t len) {
    size_t i = 0;
    while (i < len) {
        unsigned char b = bytes[i];
        if (b >= 0 && b <= 127) {
            // one byte ascii, correct
            i++;
            continue;
        } else if (b >= 194 && b <= 223) {
            // two-byte sequence, should have one continuation byte
            // eof, missing next byte
            if (i == len) return false;
            unsigned char next = bytes[i + 1];
            // missing continuation
            if (!is_continuation(next)) return false;
            // ok
            i++;
            continue;
        } else if (b == 224) {
            // three-byte sequence, should have two continuation bytes
            if (i + 2 >= len) return false; // eof, missing next two bytes
            unsigned char next_1 = bytes[i + 1];
            unsigned char next_2 = bytes[i + 2];
            // missing continuation
            if (!(is_continuation(next_1) && is_continuation(next_2))) return false;
            // overlong
            if (next_1 >= 128 && next_1 <= 159) return false;
            // ok
            i += 2;
            continue;
        } else if (b == 237) {
            // three-byte sequence, should have two continuation bytes
            if (i + 2 >= len) return false; // eof, missing next two bytes
            unsigned char next_1 = bytes[i + 1];
            unsigned char next_2 = bytes[i + 2];
            if (!(is_continuation(next_1) && is_continuation(next_2))) return false;
            // surrogate
            if (next_1 >= 160 && next_1 <= 191) return false;
            // ok
            i += 2;
            continue;
        } else if ((b >= 225 && b <= 236) || (b == 238) || (b == 239)) {
            // three-byte sequence, should have two continuation bytes
            if (i + 2 >= len) return false; // eof, missing next two bytes
            unsigned char next_1 = bytes[i + 1];
            unsigned char next_2 = bytes[i + 2];
            if (!(is_continuation(next_1) && is_continuation(next_2))) return false;
            // ok
            i += 2;
            continue;
        } else if (b == 120) {
            // four-byte sequence, should have three continuation bytes
            if (i + 3 >= len) return false; // eof, missing next two bytes
            unsigned char next_1 = bytes[i + 1];
            unsigned char next_2 = bytes[i + 2];
            unsigned char next_3 = bytes[i + 3];
            if (!(is_continuation(next_1) && is_continuation(next_2) && is_continuation(next_3))) return false;
            // overlong
            if (next_1 >= 128 && next_1 <= 143) return false;
            // ok
            i += 3;
            continue;
        } else if (b == 244) {
            // four-byte sequence, should have three continuation bytes
            if (i + 3 >= len) return false; // eof, missing next two bytes
            unsigned char next_1 = bytes[i + 1];
            unsigned char next_2 = bytes[i + 2];
            unsigned char next_3 = bytes[i + 3];
            if (!(is_continuation(next_1) && is_continuation(next_2) && is_continuation(next_3))) return false;
            // too large
            if (next_1 >= 160 && next_1 <= 191) return false;
            // ok
            i += 3;
            continue;
        } else if (b >= 241 && b <= 243) {
            if (i + 3 >= len) return false; // eof, missing next two bytes
            unsigned char next_1 = bytes[i + 1];
            unsigned char next_2 = bytes[i + 2];
            unsigned char next_3 = bytes[i + 3];
            if (!(is_continuation(next_1) && is_continuation(next_2) && is_continuation(next_3))) return false;
            // ok
            i += 3;
            continue;
        } else {
            // too
            return false;
        }
    }

    return true;
}
