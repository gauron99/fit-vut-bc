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
    KEYWORD,                // {else, float64, for, func, if, int, package, return, string, ?bool, true, false}
    IDENTIFIER,             // variable,..

    INTEGER,                // 7
    FLOAT,                  // 1.2
    STRING,                 // hello

    BINARY,                 // 0b111
    OCTAL,                  // 0o12345
    HEXA,                   // oxAF

    ASSIGNMENT,             // =
    DEFINITION,             // :=

    LEFT_ROUND_BRACKET,     // (
    RIGHT_ROUND_BRACKET,    // )

    LEFT_CURLY_BRACKET,     // {
    RIGHT_CURLY_BRACKET,    // }

    LESS,                   // <
    LESS_OR_EQUAL,          // <=
    GREATER,                // >
    GREATER_OR_EQUAL,       // >=
    EQUAL,                  // ==
    NOT_EQUAL,              // !=

    NOT,                    // !
    AND,                    // &&
    OR,                     // ||

    PLUS,                   // +
    MINUS,                  // -
    MUL,                    // *
    DIV,                    // /

    PLUS_EQ,                //+=
    MINUS_EQ,               //-=
    MUL_EQ,                 // *=
    DIV_EQ,                 // /=

    COMMA,                  //,
    SEMICOLON,              // ;

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
typedef enum { /*state num, read lexeme*/

/*TODO PRIDAT EOF DO MULTILINE KOMENTARU, TEDA VYLUCIT HO*/
    INIT_ST,            /*S*/
    ST_EOL,             /*P_5*/
    ST_PLUS,            /*O_10, + */
    ST_PLUS_EQ,         /*O_13, += */
    ST_MINUS,           /*O_11, - */
    ST_MINUS_EQ,        /*O_14, -=*/
    ST_MUL,             /*O_12 , * */
    ST_MUL_EQ,          /*O_15 , *= */
    ST_R_ROUND_BRACKET, /*P_0 , ) */
    ST_L_ROUND_BRACKET, /*P_1 , ( */
    ST_R_CURLY_BRACKET, /*P_2 , } */
    ST_L_CURLY_BRACKET, /*P_3 , { */
    ST_COMMA,           /*P_4 , , */
    ST_SEMICOLON,
    ST_LESS,            /*O_0 , < */
    ST_LESS_EQUAL,      /*O_1 , <= */
    ST_GREATER,         /*O_2 , > */
    ST_GREATER_EQUAL,   /*O_3 , >= */
    ST_ASSIGNMENT,      /*O_4 , = */
    ST_EQUAL,           /*O_5 , == */
    ST_EXCL_MARK,       /*O_6 , ! */
    ST_NOT_EQUAL,       /*O_7 , != */
    ST_COLON,           /*O_8 , : */
    ST_DEFINITION,      /*O_9 , := */
    ST_AMPERSAND,       /*O_17 , & */
    ST_AND,             /*O_18 , && */
    ST_PIPE,            /*O_19 , | */
    ST_OR,              /*O_20 , || */
    ST_SLASH,           /*C_0 , / */
    ST_SLASH_EQ,        /*O_16 , /= */
    ST_SINGLE_L_COMMENT,/*C_3, // */
    ST_MULTI_L_COMMENT, /*C_1, /~* */
    ST_MULTI_LC_PRE_END,/*C_3, /~*..* */
    ST_STRING_START,    /*S_1, " */
    ST_STRING_END,      /*S_2, "" */
    ST_STRING_ESC_START,/*S_3, "../ */
    ST_STRING_HEXA_1,   /*S_4, /x */
    ST_STRING_HEXA_2,   /*S_5, /x. */ 
    ST_NUM_WHOLE_PART_ZERO,     /*L_0, 0 */
    ST_NUM_WHOLE_PART_NONZERO,  /*L_1, 1 */
    ST_NUM_DECIMAL_POINT,       /*L_2, 1. */
    ST_NUM_FRACTIONAL_PART,     /*L_3, 1.8 */
    ST_NUM_EXPONENT,            /*L_4, 1.3E,e */
    ST_NUM_EXPONENT_POWER,      /*L_5, 1.3e1 */
    ST_IDENTIF_KEYWORD,         /*I_0, _a.. */
    ST_BINARY_BASE,             /*L_6, 0b */
    ST_BINARY_CORE,             /*L_7, 0b0 */
    ST_BINARY_SEPARATOR,        /*L_8, 0b0_ */
    ST_OCTAL_BASE,              /*L_9 0o */
    ST_OCTAL_CORE,              /*L_10, 0o7 */
    ST_OCTAL_SEPARATOR,         /*L_11, 0o7_ */
    ST_HEXA_BASE,               /*L_12, 0x */
    ST_HEXA_CORE,               /*L_13, 0xA */
    ST_HEXA_SEPARATOR           /*L_14, 0xA_ */

} stateType;




/**
 * 
 * 
 */
/* vracia navratovy kod */
int getToken(Token *token);
int ungetToken(Token *token);


#endif // !SCANNER_H