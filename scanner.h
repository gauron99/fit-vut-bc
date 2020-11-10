/** * * * * * * * * * * * * * *
 *                            *
 * @brief IFJ 2020 - Scanner  *
 *        Header file         *
 *                            *
 *  @author Sabina Gulcikova  *
 *                            *
 *      @date 17/10/20        *
 *                            *
 * * * * * * * * * * * * * *  */ 

#ifndef SCANNER_H 
#define SCANNER_H

/**
 * 
 * 
 */
typedef enum { 
    KEYWORD, 
    IDENTIFIER,

    INTEGER,
    FLOAT,
    STRING,

    ASSIGNMENT, // =
    DEFINITION, // :=

    LEFT_ROUND_BRACKET,
    RIGHT_ROUND_BRACKET,

    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,

    LESS,
    LESS_OR_EQUAL,
    GREATER,
    GREATER_OR_EQUAL,
    EQUAL,
    NOT_EQUAL,

    PLUS,
    MINUS,
    MUL,
    DIV,

    COMMA,

    EOL_,
    EOF_

} tokenType;

/**
 * 
 * 
 */
typedef struct {
    tokenType type;

    union {
        int intValue;
        float floatValue;
        char* stringValue;
    } value;
} Token;   

/**
 * 
 * 
 */
typedef enum {
    INIT_ST, 
    ST_EOL,
    ST_PLUS, //+
    ST_PLUS_EQ, //+=
    ST_MINUS, // -
    ST_MINUS_EQ, //-=
    ST_MUL, //*
    ST_MUL_EQ, //*=
    ST_R_ROUND_BRACKET, // )
    ST_L_ROUND_BRACKET, // (
    ST_R_CURLY_BRACKET, // }
    ST_L_CURLY_BRACKET, // {
    ST_COMMA, // ,
    ST_LESS, // <
    ST_LESS_EQUAL, // <=
    ST_GREATER, // >
    ST_GREATER_EQUAL, // >=
    ST_ASSIGNMENT, // =
    ST_EQUAL,  // ==
    ST_EXCL_MARK, // !
    ST_NOT_EQUAL, // !=
    ST_COLON, // :
    ST_DEFINITION, // :=
    ST_AMPERSAND, // &
    ST_AND, // &&
    ST_PIPE, // |
    ST_OR, // ||
    ST_SLASH, // /
    ST_SLASH_EQ, // /=
    ST_SINGLE_L_COMMENT, // //
    ST_MULTI_L_COMMENT, // /*
    ST_MULTI_LC_PRE_END, // /* ... *
    ST_STRING_START, // "
    ST_STRING_END, // "..."
    ST_STRING_ESC_START, // "../
    ST_STRING_HEXA_1, // ".. /x
    ST_STRING_HEXA_2, // ".. /x..
    ST_NUM_WHOLE_PART_ZERO, // 0
    ST_NUM_WHOLE_PART_NONZERO, // 1
    ST_NUM_DECIMAL_POINT, //1. 0.
    ST_NUM_FRACTIONAL_PART, //1.7
    ST_NUM_EXPONENT, //E,e 
    ST_NUM_EXPONENT_POWER, // 1.7e1
    ST_IDENTIF_KEYWORD, //_,a..
    ST_BINARY_BASE, // 0b
    ST_BINARY_CORE, // 0b0
    ST_BINARY_SEPARATOR, // 0b_
    ST_OCTAL_BASE, // 0o
    ST_OCTAL_CORE, // 0o7
    ST_OCTAL_SEPARATOR, // 0o7_
    ST_HEXA_BASE, // 0x
    ST_HEXA_CORE, // 0xA
    ST_HEXA_SEPARATOR //0xA_

} stateType;

/**
 * 
 * 
 */
/* vracia navratovy kod */
int getToken(FILE *input, Token *token);


#endif // !SCANNER_H
