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

#include "scanner.h"
#include "error.h"
#define EOL '\n'


int getToken(Token *token) {
   
    FILE *input;
    input = stdin; //source code
    int returnVal = 0;

    stateType currentState = INIT_ST;

    int c = '\0'; //TODO: mal by byt ako int?

    while((c = getc(input))) {

        switch (currentState) {
        case INIT_ST:
            if(c == EOF) { /* end of file */
                token->type = EOF_;
                token->value.stringValue = "EOF";
                return SUCCESS; 
            } 

            if(c == EOL) {  /* end of line */
                token->type = EOL_;
                token->value.stringValue = "EOL";
                return SUCCESS;
            } 

            if(c == ' ' || c == '\t') { /*whitespace*/
                break;
            } 

            if(c == '+') { /*addition operator*/
                currentState = ST_PLUS;
                ungetc(c, input);
            } 

            if(c == '-') { /*subtraction operator*/
                currentState = ST_MINUS;
                ungetc(c, input);
            } 

            if(c == '*') { /*multiplication operator*/
                currentState = ST_MUL;
                ungetc(c, input);
            } 

            if(c == ')') { 
                currentState = ST_R_ROUND_BRACKET;
                ungetc(c, input);
            } 

            if(c == '(') {
                currentState = ST_L_ROUND_BRACKET;
                ungetc(c, input);
            } 

            if(c == '}') {
                currentState = ST_R_CURLY_BRACKET;
                ungetc(c, input);
            } 

            if(c == '{') {
                currentState = ST_L_CURLY_BRACKET;
                ungetc(c, input);
            } 

            if(c == ',') {
                currentState = ST_COMMA;
                ungetc(c, input);
            } 

            if(c == '<') { /*less than*/
                currentState = ST_LESS;
            } 

            if(c == '>') { /*greater than*/
                currentState = ST_GREATER;
            }

            if(c == '=') { /*assignment/comparison/definition*/
                currentState = ST_ASSIGNMENT;
            }

            if(c == '!') { 
                currentState = ST_EXCL_MARK;
            }

            if(c == ':') {
                currentState = ST_COLON;
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
            
        /*-------------------------COMPARISON--------------------------*/
        /*------------------------------<------------------------------*/
        case ST_LESS:
            if(c == '=') {
                currentState = ST_LESS_EQUAL;
                ungetc(c, input);
            } else {
                token->type = LESS;
                token->value.stringValue = "<";
                ungetc(c, input);
                return SUCCESS;
            }
        
        /*-----------------------------<=------------------------------*/
        case ST_LESS_EQUAL:
            token->type = LESS_OR_EQUAL;
            token->value.stringValue = "<=";
            return SUCCESS;

        /*------------------------------>------------------------------*/
        case ST_GREATER:
            if(c == '=') {
                currentState = ST_GREATER_EQUAL;
                ungetc(c, input);
            } else {
                token->type = GREATER;
                token->value.stringValue = ">";
                ungetc(c, input);
                return SUCCESS;
            }

        /*----------------------------->=------------------------------*/
        case ST_GREATER_EQUAL:
            token->type = GREATER_OR_EQUAL;
            token->value.stringValue = ">=";
            return SUCCESS; 

        /*-------------------------ASSIGNMENT----------=---------------*/
        case ST_ASSIGNMENT:
            if(c == '=') {
                currentState = ST_EQUAL;
                ungetc(c, input);
            } else {
                token->type = ASSIGNMENT;
                token->value.stringValue = "=";
                ungetc(c, input);
                return SUCCESS;
            } 

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

        /*-------------------------DEFINITION-------------:=-----------*/
        case ST_DEFINITION:
            token->type = DEFINITION;
            token->value.stringValue = ":=";
            return SUCCESS;  

        default:
            break;
        }

       // printf("%c", c);
    }

    return returnVal;
}

