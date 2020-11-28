//
// Created by louda on 11/10/20.
//

#ifndef IFJ_IFJ20_H
#define IFJ_IFJ20_H

#define CHECK(arg) {int ret = arg; \
        if (ret) return ret;}
#define CHECK_R(exp,rc) if (!(exp)) return rc;

#define TTYPE token.type
#define TSTR token.value
#define TINT token.value.intValue
#define TFLOAT token.value.floatValue

typedef enum {
    EC_GOOD     = 0,
    EC_LEX      = 1,
    EC_SYN      = 2,
    EC_SEM3     = 3,
    EC_SEM4     = 4,
    EC_SEM5     = 5,
    EC_SEM6     = 6,
    EC_OTHER    = 7,
    EC_ZERODIV  = 9,
    EC_INTERNAL = 99
} returnCode;


#endif //IFJ_IFJ20_H
