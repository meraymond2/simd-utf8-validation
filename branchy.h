#ifndef UTF8_VALIDATION_BRANCHY_H
#define UTF8_VALIDATION_BRANCHY_H

#include <stdbool.h>
#include <stdlib.h>

bool is_continuation(unsigned char byte);

// 0.1 seconds for citylots.json (node took 0.13)
bool branchy_validate(const unsigned char *bytes, size_t len);

#endif //UTF8_VALIDATION_BRANCHY_H
