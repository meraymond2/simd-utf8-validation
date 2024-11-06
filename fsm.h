#ifndef UTF8_VALIDATION_FSM_H
#define UTF8_VALIDATION_FSM_H

enum {
    valid,
    one_more,
    two_more,
    three_more,
    check_3_byte_overlong,
    check_3_byte_surrogate,
    check_4_byte_overlong,
    check_4_byte_too_large,
    error // could split into types
} State;



#endif //UTF8_VALIDATION_FSM_H
