/** * * * * * * * * * * * * * *
 *                            *
 * @brief IFJ 2020 - Scanner  *
 *                            *
 *  @author Sabina Gulcikova  *
 *                            *
 *      @date 17/10/20        *
 *                            *
 * * * * * * * * * * * * * *  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "error.h"
#include "custom_string.h"

#define EOL '\n'
int linesPassed = 1; /*keeps the count of lines 
                      in order to display proper error message if problem occurs*/

int gettToken(Token *token) {

   int numOfKeywords = 9;
   char *keyWords[] = { /*array of reserved keywords*/
                    "else",
                    "float64",
                    "for",
                    "func",
                    "if",
                    "int",
                    "package",
                    "return",
                    "string"
                  };

    FILE *input;
    input = stdin; /*contains source code*/
    int returnVal = 0; /*default return value*/

    dynamicString string;
    dynamicString *content = &string; /*stores currently read token*/
    if(initDynamicString(content) == -1) {
        fprintf(stderr, "INTERNAL ERROR: malloc failed");
        return INTERNAL_ERROR;
    } 

    char hexaEscape[3];
    char c = '\0';

    stateType currentState = INIT_ST; /*beginning in initial state*/

    while((c = getc(input))) {
        switch (currentState) {
        case INIT_ST:
            if(c == EOF) { /*end of file*/
                token->type = EOF_;
                token->value = "EOF";
                return SUCCESS;
            } else if(c == EOL) { /*end of line*/
                linesPassed++;
                token->type = EOL_;
                token->value = "EOL";
                return SUCCESS;
            } else if(c == ' ' || c == '\t') { /*whitespace*/
                break;
            } else if(c == '+') { /*addition operator*/
                currentState = ST_PLUS;
            } else if(c == '-') { /*subtraction operator*/
                currentState = ST_MINUS;
            } else if(c == '*') { /*multiplication operator*/
                currentState = ST_MUL;
            } else if(c == ')') {
                currentState = ST_R_ROUND_BRACKET;
                ungetc(c, input);
            } else if(c == '(') {
                currentState = ST_L_ROUND_BRACKET;
                ungetc(c, input);
            } else if(c == '}') {
                currentState = ST_R_CURLY_BRACKET;
                ungetc(c, input);
            } else if(c == '{') {
                currentState = ST_L_CURLY_BRACKET;
                ungetc(c, input);
            } else if(c == ',') {
                currentState = ST_COMMA;
                ungetc(c, input);
            } else if(c == ';') {
                currentState = ST_SEMICOLON;
                ungetc(c, input);
            } else if(c == '<') { /*less than*/
                currentState = ST_LESS;
            } else if(c == '>') { /*greater than*/
                currentState = ST_GREATER;
            } else if(c == '=') { /*assignment/comparison/definition*/
                currentState = ST_ASSIGNMENT;
            } else if(c == '!') {
                currentState = ST_EXCL_MARK;
            } else if(c == ':') {
                currentState = ST_COLON;
            } else if(c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { /*identifier/keyword*/
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_IDENTIF_KEYWORD;
            } else if(c == '\"') { /*string*/
                currentState = ST_STRING_START;
            } else if(isdigit(c) && c == '0') { /*literal*/
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_WHOLE_PART_ZERO;
            } else if(isdigit(c) && c != '0') { /*literal*/
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_WHOLE_PART_NONZERO;
            } else if(c == '/') {  /*division/comment*/
                currentState = ST_SLASH;
            } else if(c == '&') {
                currentState = ST_AMPERSAND; /*logic and*/
            } else if(c == '|') {
                currentState = ST_PIPE; /*logic or*/
            } else {
              currentState = ST_ERROR;
            }
            break;
        /*--------------------------ADDITION---------------+-----------*/
        /*----------------------------O_10-----------------------------*/

        case ST_PLUS:
            if(c == '=') {
                currentState = ST_PLUS_EQ;
            } else {
                token->type = PLUS;
                token->value = "+";
                ungetc(c, input);
                return SUCCESS;
            }
            break;

        /*-------UNARY:-------------ADDITION----------------+=----------*/
        /*----------------------------O_13-----------------------------*/

        case ST_PLUS_EQ:
            token->type = PLUS_EQ;
            token->value = "+=";
            ungetc(c, input);
            return SUCCESS;

        /*-------------------------SUBTRACTION-------------------------*/        
        /*----------------------------O_11-----------------------------*/

        case ST_MINUS:
            if(c == '=') {
                currentState = ST_MINUS_EQ;
            } else {
                token->type = MINUS;
                token->value = "-";
                ungetc(c,input);
                return SUCCESS;
            }
            break;

        /*-------UNARY:------------SUBTRACTION---------------=---------*/
        /*----------------------------O_14-----------------------------*/

        case ST_MINUS_EQ:
            token->type = MINUS_EQ;
            token->value = "-=";
            ungetc(c, input);
            return SUCCESS;

        /*-----------------------MULTIPLICATION------------*-----------*/
        /*----------------------------O_12-----------------------------*/

        case ST_MUL:
            if(c == '=') {
                currentState = ST_MUL_EQ;
            } else {
                token->type = MUL;
                token->value = "*";
                ungetc(c, input);
                return SUCCESS;
            }
            break;

        /*------UNARY------------MULTIPLICATION------------*=----------*/
        /*----------------------------O_15-----------------------------*/


        case ST_MUL_EQ:
            token->type = MUL_EQ;
            token->value = "*=";
            ungetc(c, input);
            return SUCCESS;

        /*---------------------------COMMENT---------------------------*/
        /*-----------------------------C_0-----------------------------*/

        case ST_SLASH:
            if(c == '/') {
                currentState = ST_SINGLE_L_COMMENT;
            } else if(c == '*') {
                currentState = ST_MULTI_L_COMMENT;
            } else if(c == '=') {
                currentState = ST_SLASH_EQ;
            } else {
                token->type = DIV;
                token->value = "/";
                ungetc(c, input);
                return SUCCESS;
            }
            break;

        /*-----------------------------C_3-----------------------------*/

        case ST_SINGLE_L_COMMENT:
            if(c == EOL) {
                linesPassed++;
                token->type = EOL_;
                token->value = "EOL";

                return SUCCESS;
            } else if(c == EOF) {
                token->type = EOF_;
                token->value = "EOF";

                return SUCCESS;
            } else {
                currentState = ST_SINGLE_L_COMMENT;
            }
            break;

        /*-----------------------------C_1-----------------------------*/

        case ST_MULTI_L_COMMENT:
            if(c == '*') {
                currentState = ST_MULTI_LC_PRE_END;
            } else if(c == EOF) {
                fprintf(stderr, "LEXICAL ERROR: End of file in multi-line comment \n");
                return LEXICAL_ERROR;
            } else if(c == EOL) {
                linesPassed++;
                currentState = ST_MULTI_L_COMMENT;    
            } else {
                currentState = ST_MULTI_L_COMMENT;
            }
            break;

        /*----------------------------C_2------------------------------*/

        case ST_MULTI_LC_PRE_END:
            if(c == '/') {
                currentState = INIT_ST;
            } else if(c == EOF) {
                fprintf(stderr, "LEXICAL ERROR: End of file in multi-line comment \n");
                return LEXICAL_ERROR;
            } else if(c == '*') {
                currentState = ST_MULTI_LC_PRE_END;
            } else {
                currentState = ST_MULTI_L_COMMENT;
            }
            break;

        /*------UNARY---------------DIVISION---------------/=----------*/
        /*----------------------------O_16-----------------------------*/

        case ST_SLASH_EQ:
            token->type = DIV_EQ;
            token->value = "/=";
            ungetc(c, input);
            return SUCCESS;

        /*-----------------------ROUND BRACKETS-----------()-----------*/
        /*----------------------------P_0------------------------------*/

        case ST_R_ROUND_BRACKET:
            token->type = RIGHT_ROUND_BRACKET;
            token->value = ")";
            return SUCCESS;

        /*----------------------------P_1------------------------------*/

        case ST_L_ROUND_BRACKET:
            token->type = LEFT_ROUND_BRACKET;
            token->value = "(";
            return SUCCESS;

        /*-----------------------CURLY BRACKETS------------{}----------*/
        /*----------------------------P_2------------------------------*/

        case ST_R_CURLY_BRACKET:
            token->type = RIGHT_CURLY_BRACKET;
            token->value = "}";
            return SUCCESS;

        /*----------------------------P_3------------------------------*/

        case ST_L_CURLY_BRACKET:
            token->type = LEFT_CURLY_BRACKET;
            token->value = "{";
            return SUCCESS;

        /*-----------------------COMMA SEPARATOR-------------,---------*/
        /*----------------------------P_4------------------------------*/

        case ST_COMMA:
            token->type = COMMA;
            token->value = ",";
            return SUCCESS;

        /*-------------------------SEMICOLON----------------;----------*/
        /*----------------------------P_5------------------------------*/

        case ST_SEMICOLON:
            token->type = SEMICOLON;
            token->value = ";";
            return SUCCESS;

        /*-------------------------COMPARISON--------------------------*/
        /*----------------------------O_0--------------------<---------*/

        case ST_LESS:
            if(c == '=') {
                currentState = ST_LESS_EQUAL;
            } else {
                token->type = LESS;
                token->value = "<";
                ungetc(c, input);
                return SUCCESS;
            }

            break;

        /*----------------------------0_1-------------------<=--------*/

        case ST_LESS_EQUAL:
            token->type = LESS_OR_EQUAL;
            token->value = "<=";
            ungetc(c, input);
            return SUCCESS;

        /*----------------------------O_2--------------------->--------*/

        case ST_GREATER:
            if(c == '=') {
                currentState = ST_GREATER_EQUAL;
            } else {
                token->type = GREATER;
                token->value = ">";
                ungetc(c, input);
                return SUCCESS;
            }

            break;

        /*----------------------------O_3-------------------->=--------*/

        case ST_GREATER_EQUAL:
            token->type = GREATER_OR_EQUAL;
            token->value = ">=";
            ungetc(c, input);
            return SUCCESS;

        /*-------------------------ASSIGNMENT-----------------=--------*/
        /*----------------------------O_4------------------------------*/

        case ST_ASSIGNMENT:
            if(c == '=') {
                currentState = ST_EQUAL;
            } else {
                token->type = ASSIGNMENT;
                token->value = "=";
                ungetc(c, input);
                return SUCCESS;
            }

            break;

        /*----------------------------O_5---------------------==-------*/

        case ST_EQUAL:
            token->type = EQUAL;
            token->value = "==";
            ungetc(c, input);
            return SUCCESS;

        /*-----BOOLTHEN---------------O_6----------------------!-------*/

        case ST_EXCL_MARK:
            if(c == '=') {
                currentState = ST_NOT_EQUAL;
            } else {
                token->type = NOT;
                token->value = "!";
                ungetc(c, input);
                return SUCCESS;
            }

            break;

        /*----------------------------O_7----------------------!=------*/

        case ST_NOT_EQUAL:
            token->type = NOT_EQUAL;
            token->value = "!=";
            ungetc(c, input);
            return SUCCESS;

        /*----------------------------O_17----------------------&------*/
        
        case ST_AMPERSAND:
            if(c == '&') {
                currentState = ST_AND;
            } else {
                ungetc(c, input);
                fprintf(stderr, "LEXICAL ERROR: Invalid character '&' on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break;

        /*-----BOOLTHEN---------------O_18----------------------&&-----*/

        case ST_AND:
            token->type = AND;
            token->value = "&&";
            ungetc(c, input);
            return SUCCESS;

        /*----------------------------O_19----------------------|------*/

        case ST_PIPE:
            if(c == '|') {
                currentState = ST_OR;
            } else {
                ungetc(c, input); 
                fprintf(stderr, "LEXICAL ERROR: Invalid character '|' on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break;

        /*-----BOOLTHEN---------------O_20----------------------||-----*/

        case ST_OR:
            token->type = OR;
            token->value = "||"; 
            ungetc(c, input);
            return SUCCESS; 

        /*----------------------------O_8------------------------:-----*/

        case ST_COLON:
            if(c == '=') {
                currentState = ST_DEFINITION;
            } else {
                ungetc(c, input);
                fprintf(stderr, "LEXICAL ERROR: Invalid character ':' on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }

            break;

        /*-------------------------DEFINITION------------------:=------*/
        /*----------------------------O_9------------------------------*/

        case ST_DEFINITION:
            ungetc(c, input); //TODO
            token->type = DEFINITION;
            token->value = ":=";
            return SUCCESS;

        /*----------------------IDENTIFIER/KEYWORD---------------------*/
        /*----------------------------I_0------------------------------*/

        case ST_IDENTIF_KEYWORD:
            if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_IDENTIF_KEYWORD;
            } else {
                ungetc(c, input);
                if(appendChar(content, '\0') == -1) {
                    return INTERNAL_ERROR;
                }

                char *stored = convertToString(content); 

                if(stored == NULL) {
                    return INTERNAL_ERROR;
                }
                
                if(strcmp("true", stored) == 0) {
                    token->type = BOOL;
                    token->value = stored;

                    eraseDynamicString(content);
                    freeDynamicString(content);
                    return SUCCESS;
                } 

                if(strcmp("false", stored) == 0) {
                    token->type = BOOL;
                    token->value = stored;

                    eraseDynamicString(content);
                    freeDynamicString(content);
                    return SUCCESS;
                }

                for(int i = 0; i < numOfKeywords; i++) {
                    if(strcmp(keyWords[i], stored) == 0) {
                        token->type = KEYWORD;
                        token->value = stored;

                        eraseDynamicString(content);
                        freeDynamicString(content);
                        return SUCCESS;
                    }
                }
                token->type = IDENTIFIER;
                token->value = stored;

                eraseDynamicString(content);
                freeDynamicString(content);

                return SUCCESS;
            }

            break;

        /*----------------------------STRING---------------------------*/
        /*-----------------------------S_1-----------------------------*/

        case ST_STRING_START:
            if(c == '\"') {
                currentState = ST_STRING_END;
            } else if(c == '\\') {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_STRING_ESC_START;
            } else if(c == EOL || (c >= 0 && c <= 31) || c == EOF){
                if(c == EOL) {
                    linesPassed++;
                }
                fprintf(stderr, "LEXICAL ERROR: Invalid string on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            } else {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_STRING_START;
            }

            break;

        /*-----------------------------S_2-----------------------------*/

        case ST_STRING_END:
            ungetc(c, input);
            //here  
            if(appendChar(content, '\0') == -1) {
                return INTERNAL_ERROR;
            }

            char* stored = convertToString(content);
            if(stored == NULL) {
                return INTERNAL_ERROR;
            }

            token->type = STRING;
            token->value = stored; 

            eraseDynamicString(content);
            freeDynamicString(content);

            return SUCCESS;

        /*-----------------------------S_3-----------------------------*/

        case ST_STRING_ESC_START:
            if(c == '\"') { /* \" == \034 */
                if(appendChar(content, '0') == -1) {
                    return INTERNAL_ERROR;
                } 
                if(appendChar(content, '3') == -1) {
                    return INTERNAL_ERROR;
                } 
                if(appendChar(content, '4') == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_STRING_START;
            } else if(c == 'n') { /* \n == \010 */
                if(appendChar(content, '0') == -1) {
                    return INTERNAL_ERROR;
                }     
                if(appendChar(content, '1') == -1) {
                    return INTERNAL_ERROR;
                }  
                if(appendChar(content, '0') == -1) {
                    return INTERNAL_ERROR;
                } 
                currentState = ST_STRING_START;
            } else if(c == 't') { /* \t == \009 */
                if(appendChar(content, '0') == -1) {
                    return INTERNAL_ERROR;
                } 
                if(appendChar(content, '0') == -1) {
                    return INTERNAL_ERROR;
                } 
                if(appendChar(content, '9') == -1) {
                    return INTERNAL_ERROR;
                } 
                currentState = ST_STRING_START;
            } else if(c == '\\') { /* \\ == \092 */
                if(appendChar(content, '0') == -1) {
                    return INTERNAL_ERROR;
                }
                if(appendChar(content, '9') == -1) {
                    return INTERNAL_ERROR;
                }  
                if(appendChar(content, '2') == -1) {
                    return INTERNAL_ERROR;
                } 
                currentState = ST_STRING_START;
            } else if(c == 'x') {
                currentState = ST_STRING_HEXA_1;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid escape sequence in string on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }

            break;

        /*-----------------------------S_4-----------------------------*/

        case ST_STRING_HEXA_1:
            if(isHexa(c)) {
                hexaEscape[0] = c;
                currentState = ST_STRING_HEXA_2;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid character in hexa escape sequence on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }

            break;

        /*-----------------------------S_5-----------------------------*/

        case ST_STRING_HEXA_2:
            if(isHexa(c)) {
               hexaEscape[1] = c;
               hexaEscape[2] = '\0';

               currentState = ST_STRING_START; 

                /*here comes the converting*/
                /*
                         <`--'\>______
                        /. .  `'     \
                        (`')  ,        @
                        `-._,        /
                            )-)_/--( >  
                        ''''  ''''
                                            */
                char *ptr;
                int hexaVal = strtol(hexaEscape, &ptr, 16);

                if(hexaVal <= 9) {
                    if(appendChar(content, '0') == -1) {
                        return INTERNAL_ERROR;
                    }

                    if(appendChar(content, '0') == -1) {
                        return INTERNAL_ERROR;
                    }

                    char temp = hexaVal + 48;
                    if(appendChar(content, temp) == -1) {
                        return INTERNAL_ERROR;
                    }
                } else if(hexaVal >= 10 && hexaVal <= 99) {
                    if(appendChar(content, '0') == -1) {
                        return INTERNAL_ERROR;
                    }

                    if(appendChar(content, (hexaVal / 10) + 48) == -1) {
                        return INTERNAL_ERROR;
                    }

                    if(appendChar(content, (hexaVal % 10) + 48) == -1) {
                        return INTERNAL_ERROR;
                    }
                } else {
                    if(appendChar(content, (hexaVal / 100) + 48) == -1) {
                        return INTERNAL_ERROR;
                    } 

                    if(appendChar(content, ((hexaVal % 100) / 10) + 48) == -1) {
                        return INTERNAL_ERROR;
                    } 

                    if(appendChar(content, (hexaVal % 10) + 48) == -1) {
                        return INTERNAL_ERROR;
                    }
                }
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid character in hexa escape sequence on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }

            break;

        /*--------------------------LITERALS---------------------------*/
        /*----------------------------L_1------------------------------*/

        case ST_NUM_WHOLE_PART_NONZERO:
            if(isdigit(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_WHOLE_PART_NONZERO;
            } else if(c == '.') {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_DECIMAL_POINT;
            } else if(c == 'e' || c == 'E') {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_EXPONENT;
            } else {
                ungetc(c, input);

                //here 
                if(appendChar(content, '\0') == -1) {
                    return INTERNAL_ERROR;
                }   
                char* stored = convertToString(content);

                if(stored == NULL) {
                    return INTERNAL_ERROR;
                }

                token->type = INTEGER;
                token->value = stored;

                eraseDynamicString(content);
                freeDynamicString(content);

                return SUCCESS;
            }

            break;

        /*----------------------------L_0------------------------------*/
            
        case ST_NUM_WHOLE_PART_ZERO:
            if(c == '.') {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_DECIMAL_POINT;
            } else if(c == 'e' || c == 'E') {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_EXPONENT;
            } else if(c == 'b' || c == 'B'){
                eraseDynamicString(content);
                currentState = ST_BINARY_BASE;
            } else if(c == 'o' || c == 'O') {
                eraseDynamicString(content);
                currentState = ST_OCTAL_BASE;
            } else if(c == 'x' || c == 'X') {
                eraseDynamicString(content); 
                char *stored = convertToString(content); //?????
                if(stored == NULL) {
                    return INTERNAL_ERROR;
                }

                currentState = ST_HEXA_BASE;
            } else if(isdigit(c)) {
                fprintf(stderr, "LEXICAL ERROR: Invalid literal - leading with zero on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            } else {
                ungetc(c, input);
                //here 
                if(appendChar(content, '\0') == -1) {
                    return INTERNAL_ERROR;
                } 
                char* stored = convertToString(content); 

                if(stored == NULL) {
                    return LEXICAL_ERROR;
                }

                token->type = INTEGER;
                token->value = stored; 

                eraseDynamicString(content);
                freeDynamicString(content);

                return SUCCESS;
            }
            break;

        /*-----------------------------L_2-----------------------------*/

        case ST_NUM_DECIMAL_POINT:
            if(isdigit(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_FRACTIONAL_PART;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid literal - terminated by '.' on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }

            break;

        /*-----------------------------L_3-----------------------------*/

        case ST_NUM_FRACTIONAL_PART:
            if(isdigit(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_FRACTIONAL_PART;
            } else if(c == 'e' || c == 'E') {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_EXPONENT;
            } else {
                ungetc(c, input);
                //here 
                if(appendChar(content, '\0') == -1) {
                    return INTERNAL_ERROR;
                }
                char* stored = convertToString(content);

                if(stored == NULL) {
                    return INTERNAL_ERROR;
                }

                token->type = FLOAT;
                token->value = stored;

                eraseDynamicString(content);
                freeDynamicString(content);
                return SUCCESS;
            }

            break;

        /*-----------------------------L_4-----------------------------*/

        case ST_NUM_EXPONENT:
            if(isdigit(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_EXPONENT_POWER;
            } else if(c == '+' || c == '-') {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_EXPONENT_MUST;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid literal - missing exponent on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }

            break;

        /*-----------------------------L_6-----------------------------*/

        case ST_NUM_EXPONENT_MUST:
            if(isdigit(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_EXPONENT_POWER;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid literal - invalid exponent on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break;
        /*-----------------------------L_5-----------------------------*/

        case ST_NUM_EXPONENT_POWER:
            if(isdigit(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_NUM_EXPONENT_POWER;
            } else {
                ungetc(c, input);
                //here 
                if(appendChar(content, '\0') == -1) {
                    return INTERNAL_ERROR;
                }
                char* stored = convertToString(content);

                if(stored == NULL) {
                    return INTERNAL_ERROR;
                }

                token->type = FLOAT;
                token->value = stored;

                eraseDynamicString(content);
                freeDynamicString(content);
                return SUCCESS;
            }

            break; 

        /*---------------------------BINARY----------------------------*/
        /*--------BASE----------------L_7------------------------------*/

        case ST_BINARY_BASE:
            if(isBinary(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_BINARY_CORE;
            } else if(c == '_') {
                currentState = ST_BINARY_SEPARATOR;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid digit in binary on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break;
        /*-----------------------------L_8-----------------------------*/

        case ST_BINARY_CORE:
            if(isBinary(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_BINARY_CORE;
            } else if(c == '_') {
                currentState = ST_BINARY_SEPARATOR;
            } else {
                ungetc(c, input);
                /*converting string containing binary number to integer*/
                char *ptr;
                int binaryValue = strtol(content->str, &ptr, 2);
                /*converting integer value into string*/

                int length = snprintf(NULL, 0, "%d", binaryValue);
                char* converted = malloc(length +1);
                if(converted == NULL) {
                    fprintf(stderr, "INTERNAL ERROR: malloc failed");
                    return INTERNAL_ERROR;
                }
                snprintf(converted, length + 1, "%d", binaryValue);
                //tuto co akoze?
                token->type = INTEGER;
                token->value = converted;

                //free(converted);
                return SUCCESS;
            }
            break; 
        /*-----------------------------L_9-----------------------------*/

        case ST_BINARY_SEPARATOR:
            if(isBinary(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_BINARY_CORE;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid digit in binary base on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break;    
            
        /*----------------------------OCTAL----------------------------*/
        /*--------BASE----------------L-10-----------------------------*/

        case ST_OCTAL_BASE:
            if(isOctal(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_OCTAL_CORE;
            } else if(c == '_') {
                currentState = ST_OCTAL_SEPARATOR;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid digit in octal base on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break; 

        /*----------------------------L_11-----------------------------*/

        case ST_OCTAL_CORE:
            if(isOctal(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_OCTAL_CORE;
            } else if(c == '_') {
                currentState = ST_OCTAL_SEPARATOR;
            } else {
                ungetc(c, input);
                char *ptr;
                int octalValue = strtol(content->str, &ptr, 8);
                int length = snprintf(NULL, 0, "%d", octalValue);
                char* converted = malloc(length +1);
                if(converted == NULL) {
                    fprintf(stderr, "INTERNAL ERROR: malloc failed");
                    return INTERNAL_ERROR;
                }
                snprintf(converted, length + 1, "%d", octalValue);

                token->type = INTEGER;
                token->value = converted;

                //free(converted)
                return SUCCESS;
            }
            break;  

        /*----------------------------L_12-----------------------------*/

        case ST_OCTAL_SEPARATOR:
            if(isOctal(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_OCTAL_CORE;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid digit in octal base on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break;     

        /*-------------------------HEXADECIMAL-------------------------*/
        /*--------BASE----------------L_13-----------------------------*/

        case ST_HEXA_BASE:
            if(isHexa(c)) { 
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_HEXA_CORE;
            } else if(c == '_') {
                currentState = ST_HEXA_SEPARATOR;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid digit in hexa base on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break;

        /*----------------------------L_14-----------------------------*/

        case ST_HEXA_CORE:
            if(isHexa(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_HEXA_CORE;
            } else if(c == '_') {
                currentState = ST_HEXA_SEPARATOR;
            } else {
                ungetc(c, input);
                /*converting string containing hexa number to integer*/
                char *ptr;
                int hexaValue = strtol(content->str, &ptr, 16);

                int length = snprintf(NULL, 0, "%d", hexaValue);
                char* converted = malloc(length +1);
                if(converted == NULL) {
                    fprintf(stderr, "INTERNAL ERROR: malloc failed");
                    return INTERNAL_ERROR;
                }
                snprintf(converted, length + 1, "%d", hexaValue);

                token->type = INTEGER;
                token->value = converted;
                
                //free(converted);
                return SUCCESS;
            }
            break;    

        /*----------------------------L_15-----------------------------*/

        case ST_HEXA_SEPARATOR:
            if(isHexa(c)) {
                if(appendChar(content, c) == -1) {
                    return INTERNAL_ERROR;
                }
                currentState = ST_HEXA_CORE;
            } else {
                fprintf(stderr, "LEXICAL ERROR: Invalid digit in hexa base on line %d \n", linesPassed);
                return LEXICAL_ERROR;
            }
            break;

        /*-------------------------------------------------------------*/    
        /*-------------------------------------------------------------*/

        case ST_ERROR:
            fprintf(stderr, "LEXICAL ERROR: Unexpected character on line %d \n", linesPassed);
          return LEXICAL_ERROR;
          
        default:
            fprintf(stderr, "LEXICAL ERROR: Unexpected character on line %d \n", linesPassed);
            return LEXICAL_ERROR;
        }
    }
    return returnVal;
} 

int isBinary(char c) {
    if(isdigit(c) && (c == '0' || c == '1')) {
        return 1;
    } else {
        return 0;
    }
}

int isOctal(char c) {
    if(isdigit(c) && (c >= '0' && c <= '7')) {
        return 1;
    } else {
        return 0;
    }
}

int isHexa(char c) {
    if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
        return 1;
    } else {
        return 0;
    }
}
