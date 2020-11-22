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
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "error.h"
#include "custom_string.h"

#define EOL '\n'

Token tkns[2];
int ungot = 0;

int ungetToken(Token *token) {
    tkns[ungot++] = *token;
};

int getToken(Token *token) {

    if(ungot) {
        *token = tkns[--ungot];
        return 0;
    }

   int numOfKeywords = 9;
   char *keyWords[] = {
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
    input = stdin; //source code
    int returnVal = 0;

    dynamicString string;
    dynamicString *content = &string;
    if (initDynamicString(content) == -1) {
        return INTERNAL_ERROR;
    }

    stateType currentState = INIT_ST;

    char c = '\0';

    while((c = getc(input))) {
        switch (currentState) {
        case INIT_ST:
            if(c == EOF) { /* end of file */
                token->type = EOF_;
                token->value.stringValue = "EOF";
                return SUCCESS;
            } else if(c == EOL) {  /* end of line */
                token->type = EOL_;
                token->value.stringValue = "EOL";
                return SUCCESS;
            } else if(c == ' ' || c == '\t') { /*whitespace*/
                break;
            } else if(c == '+') { /*addition operator*/
                currentState = ST_PLUS;
                ungetc(c, input);
            } else if(c == '-') { /*subtraction operator*/
                currentState = ST_MINUS;
                ungetc(c, input);
            } else if(c == '*') { /*multiplication operator*/
                currentState = ST_MUL;
                ungetc(c, input);
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
            } else if(c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { /*identifier*/
                appendChar(content, c);
                currentState = ST_IDENTIF_KEYWORD;
            } else if(c == '\"') { /*string*/
                currentState = ST_STRING_START;
            } else if(isdigit(c) && c == '0') { /*literal*/
                appendChar(content, c);
                currentState = ST_NUM_WHOLE_PART_ZERO;
            } else if(isdigit(c) && c != '0') { /*literal*/
                appendChar(content, c);
                currentState = ST_NUM_WHOLE_PART_NONZERO;
            } else if(c == '/') {  /*division/comment*/
                currentState = ST_SLASH;
            } else {
              currentState = ST_ERROR;
            }
            break;
        /*--------------------------ADDITION---------------+-----------*/
        case ST_PLUS:
            token->type = PLUS;
            token->value.stringValue = "+";
            return SUCCESS;


        /*-------------------------SUBTRACTION-------------------------*/
        case ST_MINUS:
            token->type = MINUS;
            token->value.stringValue = "-";
            return SUCCESS;

        /*-----------------------MULTIPLICATION------------*-----------*/
        case ST_MUL:
            token->type = MUL;
            token->value.stringValue = "*";
            return SUCCESS;

        case ST_SLASH:
            if (c == '/') {
                currentState = ST_SINGLE_L_COMMENT;
            } else if (c == '*') {
                currentState = ST_MULTI_L_COMMENT;
            } else {
                ungetc(c, input);
                token->type = DIV;
                token->value.stringValue = "/";

                return SUCCESS;
            }
            break;

        case ST_SINGLE_L_COMMENT:
            if(c == EOL) {
                token->type = EOL_;
                token->value.stringValue = "EOL";

                return SUCCESS;
            } else if(c == EOF) {
                token->type = EOF_;
                token->value.stringValue = "EOF";

                return SUCCESS;
            } else {
                currentState = ST_SINGLE_L_COMMENT;
            }
            break;

        case ST_MULTI_L_COMMENT:
            if(c == '*') {
                currentState = ST_MULTI_LC_PRE_END;
            } else if (c == EOF) {
                return LEXICAL_ERROR;
            } else {
                currentState = ST_MULTI_L_COMMENT;
            }
            break;

        case ST_MULTI_LC_PRE_END:
            if(c == '/') {
                currentState = INIT_ST;
            } else if (c == EOF) {
                return LEXICAL_ERROR;
            } else if (c == '*') {
                currentState = ST_MULTI_LC_PRE_END;
            } else {
                currentState = ST_MULTI_L_COMMENT;
            }
            break;

        /*-----------------------ROUND BRACKETS-----------()-----------*/
        case ST_R_ROUND_BRACKET:
            token->type = RIGHT_ROUND_BRACKET;
            token->value.stringValue = ")";
            return SUCCESS;

        case ST_L_ROUND_BRACKET:
            token->type = LEFT_ROUND_BRACKET;
            token->value.stringValue = "(";
            return SUCCESS;

        /*-----------------------CURLY BRACKETS------------{}----------*/
        case ST_R_CURLY_BRACKET:
            token->type = RIGHT_CURLY_BRACKET;
            token->value.stringValue = "}";
            return SUCCESS;

        case ST_L_CURLY_BRACKET:
            token->type = LEFT_CURLY_BRACKET;
            token->value.stringValue = "{";
            return SUCCESS;

        /*----------------------COMMA SEPARATOR-------------,----------*/
        case ST_COMMA:
            token->type = COMMA;
            token->value.stringValue = ",";
            return SUCCESS;

        /*----------------------SEMICOLON-------------;----------*/
        case ST_SEMICOLON:
            token->type = SEMICOLON;
            token->value.stringValue = ";";
            return SUCCESS;

        /*-------------------------COMPARISON--------------------------*/
        /*------------------------------<------------------------------*/
        case ST_LESS:
            if(c == '=') {
                currentState = ST_LESS_EQUAL;
            } else {
                token->type = LESS;
                token->value.stringValue = "<";
                ungetc(c, input);
                return SUCCESS;
            }

            break;

        /*-----------------------------<=------------------------------*/
        case ST_LESS_EQUAL:
            token->type = LESS_OR_EQUAL;
            token->value.stringValue = "<=";
            return SUCCESS;

        /*------------------------------>------------------------------*/
        case ST_GREATER:
            if(c == '=') {
                currentState = ST_GREATER_EQUAL;
            } else {
                token->type = GREATER;
                token->value.stringValue = ">";
                ungetc(c, input);
                return SUCCESS;
            }

            break;

        /*----------------------------->=------------------------------*/
        case ST_GREATER_EQUAL:
            token->type = GREATER_OR_EQUAL;
            token->value.stringValue = ">=";
            return SUCCESS;

        /*-------------------------ASSIGNMENT----------=---------------*/
        case ST_ASSIGNMENT:
            if(c == '=') {
                currentState = ST_EQUAL;
            } else {
                token->type = ASSIGNMENT;
                token->value.stringValue = "=";
                ungetc(c, input);
                return SUCCESS;
            }

            break;

        /*-----------------------------==------------------------------*/
        case ST_EQUAL:
            token->type = EQUAL;
            token->value.stringValue = "==";
            return SUCCESS;

        /*------------------------------!------------------------------*/
        case ST_EXCL_MARK:
            if(c == '=') {
                currentState = ST_NOT_EQUAL;
            } else {
                ungetc(c, input);
                return LEXICAL_ERROR;
            }

            break;

        /*------------------------------!=-----------------------------*/
        case ST_NOT_EQUAL:
            token->type = NOT_EQUAL;
            token->value.stringValue = "!=";
            return SUCCESS;

        /*------------------------------:------------------------------*/
        case ST_COLON:
            if(c == '=') {
                currentState = ST_DEFINITION;
            } else {
                ungetc(c, input);
                return LEXICAL_ERROR;
            }

            break;

        /*-------------------------DEFINITION-------------:=-----------*/
        case ST_DEFINITION:
            ungetc(c, input);
            token->type = DEFINITION;
            token->value.stringValue = ":=";
            return SUCCESS;

        /*----------------------IDENTIFIER/KEYWORD---------------------*/
        case ST_IDENTIF_KEYWORD:
            if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
                appendChar(content, c);
                currentState = ST_IDENTIF_KEYWORD;
            } else {
                ungetc(c, input);

                char *stored = convertToString(content);

                for(int i = 0; i < numOfKeywords; i++) {
                    if(strcmp(keyWords[i], stored) == 0) {
                        token->type = KEYWORD;
                        token->value.stringValue = stored;

                        eraseDynamicString(content);
                        freeDynamicString(content);
                        return SUCCESS;
                    }
                }
                token->type = IDENTIFIER;
                token->value.stringValue = stored;

                eraseDynamicString(content);
                freeDynamicString(content);

                return SUCCESS;
            }

            break;

        case ST_STRING_START:
            if(c == '\"') {
                currentState = ST_STRING_END;
            } else if(c == '\\') {
                appendChar(content, c);
                currentState = ST_STRING_ESC_START;
            } else if(c == EOL || (c >= 0 && c <= 31) || c == EOF){
                return LEXICAL_ERROR;
            } else {
                appendChar(content, c);
                currentState = ST_STRING_START;
            }

            break;

        case ST_STRING_END:
            ungetc(c, input);

            token->type = STRING;
            token->value.stringValue = content->str;

            return SUCCESS;

        case ST_STRING_ESC_START:
            if(c == '\"' || c == 'n' || c == 't' || c == '\\') {
                appendChar(content, c);
                currentState = ST_STRING_START;
            } else if(c == 'x') {
                appendChar(content, c);
                currentState = ST_STRING_HEXA_1;
            } else {
                return LEXICAL_ERROR;
            }

            break;

        case ST_STRING_HEXA_1:
            if(isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                appendChar(content, c);
                currentState = ST_STRING_HEXA_2;
            } else {
                return LEXICAL_ERROR;
            }

            break;

        case ST_STRING_HEXA_2:
            if(isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                appendChar(content, c);
                currentState = ST_STRING_START;
            } else {
                return LEXICAL_ERROR;
            }

            break;

        case ST_NUM_WHOLE_PART_NONZERO:
            if(isdigit(c)) {
                appendChar(content, c);
                currentState = ST_NUM_WHOLE_PART_NONZERO;
            } else if(c == '.') {
                appendChar(content, c);
                currentState = ST_NUM_DECIMAL_POINT;
            } else if(c == 'e' || c == 'E') {
                appendChar(content, c);
                currentState = ST_NUM_EXPONENT;
            } else {
                ungetc(c, input);

                token->type = INTEGER;
                int iNumber = atoi(content->str);
                token->value.intValue = iNumber;

                return SUCCESS;
            }

            break;

        case ST_NUM_WHOLE_PART_ZERO:
            if(c == '.') {
                appendChar(content, c);
                currentState = ST_NUM_DECIMAL_POINT;
            } else if(c == 'e' || c == 'E') {
                appendChar(content, c);
                currentState = ST_NUM_EXPONENT;
            } else if(isdigit(c)) {
                return LEXICAL_ERROR;
            } else {
                ungetc(c, input);
                token->type = INTEGER;
                int iNumber = atoi(content->str);
                token->value.intValue = iNumber;

                return SUCCESS;
            }

            break;

        case ST_NUM_DECIMAL_POINT:
            if(isdigit(c)) {
                appendChar(content, c);
                currentState = ST_NUM_FRACTIONAL_PART;
            } else {
                return LEXICAL_ERROR;
            }

            break;

        case ST_NUM_FRACTIONAL_PART:
            if(isdigit(c)) {
                appendChar(content, c);
                currentState = ST_NUM_FRACTIONAL_PART;
            } else if(c == 'e' || c == 'E') {
                appendChar(content, c);
                currentState = ST_NUM_EXPONENT;
            } else {
                ungetc(c, input);
                token->type = FLOAT;

                double fNumber = atof(content->str);
                token->value.floatValue = fNumber;

                return SUCCESS;
            }

            break;

        case ST_NUM_EXPONENT:
            if(isdigit(c)) {
                appendChar(content, c);
                currentState = ST_NUM_EXPONENT_POWER;
            } else if (c == '+' || c == '-') {
                appendChar(content, c);
                currentState = ST_NUM_EXPONENT_MUST;
            } else {
                return LEXICAL_ERROR;
            }

            break;

        case ST_NUM_EXPONENT_MUST:
            if(isdigit(c)) {
                appendChar(content, c);
                currentState = ST_NUM_EXPONENT_POWER;
            } else {
                return LEXICAL_ERROR;
            }

        case ST_NUM_EXPONENT_POWER:
            if(isdigit(c)) {
                appendChar(content, c);
                currentState = ST_NUM_EXPONENT_POWER;
            } else {
                ungetc(c, input);
                double fNumber = atof(content->str);
                int truncated = (int)fNumber;

                if (fNumber == truncated) {
                    token->type = INTEGER;
                    token->value.intValue = truncated;
                } else {
                    token->type = FLOAT;
                    token->value.floatValue = fNumber;
                }

                return SUCCESS;
            }

            break;
        case ST_ERROR:
          return LEXICAL_ERROR;
        default:
            return LEXICAL_ERROR;
        }
    }

    return returnVal;
}
