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

enum {
    ascii,
    double_prefix,
    triple_prefix,
    quad_prefix,
    triple_check_overlong,
    triple_check_surrogate,
    quad_check_overlong,
    quad_check_too_large,
    continuation_low,
    continuation_mid,
    continuation_high,
    invalid,
} Category;

#endif //UTF8_VALIDATION_FSM_H
