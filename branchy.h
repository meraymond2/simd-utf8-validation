
#ifndef UTF8_VALIDATION_BRANCHY_H
#define UTF8_VALIDATION_BRANCHY_H

#include <stdbool.h>
#include <stdlib.h>

bool is_continuation(unsigned char byte);

bool branchy_validate(const char *bytes, size_t len);

#endif //UTF8_VALIDATION_BRANCHY_H
