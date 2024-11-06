#ifndef UTF8_VALIDATION_FSM_H
#define UTF8_VALIDATION_FSM_H

#include <stdbool.h>
#include <stdlib.h>

// 0.15 seconds, slower than branchy on big files, but faster on small files
bool fsm_validate(const char *bytes, size_t len);

#endif //UTF8_VALIDATION_FSM_H
