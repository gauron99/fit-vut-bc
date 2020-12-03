/**
        ~~ IFJ-20 ~~
    @file   generator.c
    @brief  take 3AC (or some form of it) and generate code in IJFcode20
    @date   11/11/20
    @author David Fridrich
*/
#include <stdio.h>
#include "generator.h"


void generateBeginningOfExistence(){
    printf(".IFJCODE20\n");
    printf("JUMP MAIN\n\n");
    generateInBuilt();
    printf("LABEL MAIN\n");
    printf("CREATEFRAME\n");
    printf("CLEARS\n");

    return;
}
/*
func inputs() (string,int)
func inputi() (int,int)
func inputf() (float64,int)
func print(term,term,...)

func int2float( i int) (float64)
func float2int(f float64) (int)

func len(s string)(int)
func substr(s string, i int, n int) (string, int)
func ord( s string, i int)(int,int)
func chr(i int)(string,int)
*/
void generateInBuilt(){
    printf("#function inputs() !d\n");
    printf("LABEL $$__inputs__$$ #0,2\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@&&&out_arg1\n");
    printf("DEFVAR LF@&&&out_arg2\n");
    printf("DEFVAR LF@$$&&%%&%%&&$$type\n");
    printf("MOVE LF@&&&out_arg2 int@0\n");
    printf("READ LF@&&&out_arg1 string\n");
    printf("TYPE LF@$$&&%%&%%&&$$type LF@&&&out_arg1\n");
    printf("JUMPIFEQ inputs_label_end_good string@string LF@$$&&%%&%%&&$$type\n");
    printf("MOVE LF@&&&out_arg2 int@1\n");
    printf("LABEL inputs_label_end_good\n");
    printf("PUSHS LF@&&&out_arg1\n");
    printf("PUSHS LF@&&&out_arg2\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("\n");
    printf("#function inputi() !d\n");
    printf("LABEL $$__inputi__$$ #0,2\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@&&&out_arg1\n");
    printf("DEFVAR LF@&&&out_arg2\n");
    printf("DEFVAR LF@$$&&%%&%%&&$$type\n");
    printf("MOVE LF@&&&out_arg2 int@0\n");
    printf("READ LF@&&&out_arg1 int\n");
    printf("TYPE LF@$$&&%%&%%&&$$type LF@&&&out_arg1\n");
    printf("JUMPIFEQ inputi_label_end_good LF@$$&&%%&%%&&$$type string@int\n");
    printf("MOVE LF@&&&out_arg2 int@1\n");
    printf("LABEL inputi_label_end_good\n");
    printf("PUSHS LF@&&&out_arg1\n");
    printf("PUSHS LF@&&&out_arg2\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("\n");
    printf("LABEL $$__inputf__$$ #0,2 !done\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@&&&out_arg1\n");
    printf("DEFVAR LF@&&&out_arg2\n");
    printf("DEFVAR LF@$$&&%%&%%&&$$type\n");
    printf("MOVE LF@&&&out_arg2 int@0\n");
    printf("READ LF@&&&out_arg1 float\n");
    printf("TYPE LF@$$&&%%&%%&&$$type LF@&&&out_arg1\n");
    printf("JUMPIFEQ inputf_label_end_good LF@$$&&%%&%%&&$$type string@float\n");
    printf("MOVE LF@&&&out_arg2 int@1\n");
    printf("LABEL inputf_label_end_good\n");
    printf("PUSHS LF@&&&out_arg1\n");
    printf("PUSHS LF@&&&out_arg2\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("\n");
    printf("#rozsireni bool \n");
    printf("LABEL $$__inputb__$$ #0,2 !done\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@&&&out_arg1\n");
    printf("DEFVAR LF@&&&out_arg2\n");
    printf("DEFVAR LF@$$&&%%&%%&&$$type\n");
    printf("MOVE LF@&&&out_arg2 int@0\n");
    printf("READ LF@&&&out_arg1 bool\n");
    printf("TYPE LF@$$&&%%&%%&&$$type LF@&&&out_arg1\n");
    printf("JUMPIFEQ inputb_label_end_good LF@$$&&%%&%%&&$$type string@bool\n");
    printf("MOVE LF@&&&out_arg2 int@1\n");
    printf("LABEL inputb_label_end_good\n");
    printf("PUSHS LF@&&&out_arg1 #push arguments on the stack to be used\n");
    printf("PUSHS LF@&&&out_arg2\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("\n");
    printf("#int2float func !done\n");
    printf("LABEL $$__int2float__$$ #1,1 #assume input arg is on the stack\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@&&&?%%%%***!!!in_arg1\n");
    printf("DEFVAR LF@&&&?%%%%***!!!out_arg2\n");
    printf("POPS LF@&&&?%%%%***!!!in_arg1\n");
    printf("INT2FLOAT LF@&&&?%%%%***!!!out_arg2 LF@&&&?%%%%***!!!in_arg1\n");
    printf("PUSHS LF@&&&?%%%%***!!!out_arg2\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("\n");
    printf("#float2int func !done\n");
    printf("LABEL $$__float2int__$$ #1,1\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@&&&?%%%%***!!!in_arg1\n");
    printf("DEFVAR LF@&&&?%%%%***!!!out_arg2\n");
    printf("POPS LF@&&&?%%%%***!!!in_arg1\n");
    printf("FLOAT2INT LF@&&&?%%%%***!!!out_arg2 LF@&&&?%%%%***!!!in_arg1\n");
    printf("PUSHS LF@&&&?%%%%***!!!out_arg2\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("\n");
    printf("#func len (string-in,int-o\n");
    printf("LABEL $$__len__$$ #1,1\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@&&&?%%%%***!!!in_arg1\n");
    printf("DEFVAR LF@&%%$%%**!!$$retVal\n");
    printf("POPS LF@&&&?%%%%***!!!in_arg1 #string\n");
    printf("DPRINT LF@&&&?%%%%***!!!in_arg1\n");
    printf("STRLEN LF@&%%$%%**!!$$retVal LF@&&&?%%%%***!!!in_arg1\n");
    printf("PUSHS LF@&%%$%%**!!$$retVal\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("\n");
    printf("#func substr (string,int,int-in) (string, int-out) #chars start wit\n");
    printf("LABEL $$__substr__$$ #3,2\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$%%**!!$$%%%%!$in1_s\n");
    printf("DEFVAR LF@$$%%**!!$$%%%%!$in2_i\n");
    printf("DEFVAR LF@$$%%**!!$$%%%%!$in3_n\n");
    printf("POPS LF@$$%%**!!$$%%%%!$in3_n #jak dlouhy bude final string\n");
    printf("POPS LF@$$%%**!!$$%%%%!$in2_i #index zacatku noveho stringu\n");
    printf("POPS LF@$$%%**!!$$%%%%!$in1_s #hlavni string\n");
    printf("DEFVAR LF@%%%%&&***!!!!out1_s\n");
    printf("MOVE LF@%%%%&&***!!!!out1_s string@\n");
    printf("DEFVAR LF@%%%%&&***!!!!out1_err\n");
    printf("MOVE LF@%%%%&&***!!!!out1_err int@0 #chybny vystup nastaven na 0 -> success\n");
    printf("\n");
    printf("## check if length is in bounds ##\n");
    printf("DEFVAR LF@typerino\n");
    printf("MOVE LF@typerino bool@false\n");
    printf("\n");
    printf("#check n (legth of the new stri\n");
    printf("LT LF@typerino LF@$$%%**!!$$%%%%!$in3_n int@0\n");
    printf("JUMPIFEQ substr__oh_god_error_to_the_max LF@typerino bool@true\n");
    printf("DEFVAR LF@strLength\n");
    printf("STRLEN LF@strLength LF@$$%%**!!$$%%%%!$in1_s\n");
    printf("\n");
    printf("DPRINT LF@strLength\n");
    printf("\n");
    printf("LT LF@typerino LF@$$%%**!!$$%%%%!$in2_i int@0\n");
    printf("JUMPIFEQ substr__oh_god_error_to_the_max LF@typerino bool@true\n");
    printf("GT LF@typerino LF@$$%%**!!$$%%%%!$in2_i LF@strLength\n");
    printf("JUMPIFEQ substr__oh_god_error_to_the_max LF@typerino bool@true\n");
    printf("DEFVAR LF@combinedLength\n");
    printf("ADD LF@combinedLength LF@$$%%**!!$$%%%%!$in3_n LF@$$%%**!!$$%%%%!$in2_i\n");
    printf("GT LF@typerino LF@combinedLength LF@strLength\n");
    printf("JUMP substr__lookin_mighty_fine #everything is fine, change the string now\n");
    printf("\n");
    printf("LABEL substr__oh_god_error_to_the_max #error kosticka\n");
    printf("MOVE LF@%%%%&&***!!!!out1_err int@1\n");
    printf("MOVE LF@%%%%&&***!!!!out1_s string@\n");
    printf("JUMP substr__total_ending_no_spoilers #error konec kosticky \n");
    printf("\n");
    printf("#hlavni cyklus\n");
    printf("LABEL substr__lookin_mighty_fine\n");
    printf("DEFVAR LF@currentCharIndex\n");
    printf("MOVE LF@currentCharIndex LF@$$%%**!!$$%%%%!$in2_i\n");
    printf("DEFVAR LF@n_iterator\n");
    printf("DEFVAR LF@oneChar\n");
    printf("MOVE LF@n_iterator int@0\n");
    printf("LABEL substr__cyklus_maximus\n");
    printf("JUMPIFEQ substr__total_ending_no_spoilers LF@n_iterator LF@$$%%**!!$$%%%%!$in3_n #if(n ==\n");
    printf("JUMPIFEQ substr__total_ending_no_spoilers LF@currentCharIndex LF@strLength #if(i==length of s\n");
    printf("GETCHAR LF@oneChar LF@$$%%**!!$$%%%%!$in1_s LF@currentCharIndex\n");
    printf("CONCAT LF@%%%%&&***!!!!out1_s LF@%%%%&&***!!!!out1_s LF@oneChar\n");
    printf("ADD LF@currentCharIndex LF@currentCharIndex int@1\n");
    printf("ADD LF@n_iterator LF@n_iterator int@1\n");
    printf("JUMP substr__cyklus_maximus\n");
    printf("\n");
    printf("LABEL substr__total_ending_no_spoilers\n");
    printf("PUSHS LF@%%%%&&***!!!!out1_s\n");
    printf("PUSHS LF@%%%%&&***!!!!out1_err\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("#func ord() # indexing starts with 1 COZ FLYGNA WOTCH \n");
    printf("LABEl $$__ord__$$ #2,2\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$&**!%%$*&returnVal\n");
    printf("DEFVAR LF@$&**!%%$*&errorr\n");
    printf("MOVE LF@$&**!%%$*&errorr int@0\n");
    printf("DEFVAR LF@strLenMinusOne #this bad boy because its indexed NOT FROM 0\n");
    printf("\n");
    printf("DEFVAR LF@!$$!*&$!!$_s #string\n");
    printf("DEFVAR LF@!$$!*&$!!$_i #index\n");
    printf("POPS LF@!$$!*&$!!$_i\n");
    printf("POPS LF@!$$!*&$!!$_s\n");
    printf("STRLEN LF@strLenMinusOne LF@!$$!*&$!!$_s \n");
    printf("SUB LF@strLenMinusOne LF@strLenMinusOne int@1 #this because its indexed NOT FROM 0\n");
    printf("\n");
    printf("#check what we got\n");
    printf("DEFVAR LF@isDisErrorDoe\n");
    printf("MOVE LF@isDisErrorDoe bool@false\n");
    printf("LT LF@isDisErrorDoe LF@!$$!*&$!!$_i int@0\n");
    printf("JUMPIFEQ ord__oh_no_an_eerroorr LF@isDisErrorDoe bool@true\n");
    printf("GT LF@isDisErrorDoe LF@!$$!*&$!!$_i LF@strLenMinusOne\n");
    printf("JUMPIFEQ ord__oh_no_an_eerroorr LF@isDisErrorDoe bool@true\n");
    printf("JUMP ord__lookin_good_justdo_the_conversion_now\n");
    printf("\n");
    printf("#error kosticka\n");
    printf("LABEL ord__oh_no_an_eerroorr\n");
    printf("MOVE LF@$&**!%%$*&returnVal int@0 #useless tho\n");
    printf("MOVE LF@$&**!%%$*&errorr int@1\n");
    printf("JUMP ord__letsgoo_this_is_the_end\n");
    printf("\n");
    printf("#vsecko cajk, muzem konvertovat\n");
    printf("LABEL ord__lookin_good_justdo_the_conversion_now\n");
    printf("STRI2INT LF@$&**!%%$*&returnVal LF@!$$!*&$!!$_s LF@!$$!*&$!!$_i\n");
    printf("\n");
    printf("#tohle je konec\n");
    printf("LABEL ord__letsgoo_this_is_the_end\n");
    printf("PUSHS LF@$&**!%%$*&returnVal\n");
    printf("PUSHS LF@$&**!%%$*&errorr\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("################################################################################\n");
    printf("\n");
    printf("#func chr() change number (0-255) to c\n");
    printf("LABEL $$__chr__$$ #1,2\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$!&*%%&!*$!&*%%$$_i\n");
    printf("DEFVAR LF@$!&*%%&!*$!&*%%$$retVal\n");
    printf("DEFVAR LF@$!&*%%&!*$!&*%%$$ErrorVal\n");
    printf("MOVE LF@$!&*%%&!*$!&*%%$$ErrorVal int@0\n");
    printf("DEFVAR LF@errorik\n");
    printf("MOVE LF@errorik bool@false\n");
    printf("POPS LF@$!&*%%&!*$!&*%%$$_i\n");
    printf("\n");
    printf("#check if out of bounds or nah\n");
    printf("LT LF@errorik LF@$!&*%%&!*$!&*%%$$_i int@0\n");
    printf("JUMPIFEQ chr_only_one_error_ohno LF@errorik bool@true\n");
    printf("GT LF@errorik LF@$!&*%%&!*$!&*%%$$_i int@255\n");
    printf("JUMPIFEQ chr_only_one_error_ohno LF@errorik bool@true\n");
    printf("\n");
    printf("#all is well do the thing now\n");
    printf("INT2CHAR LF@$!&*%%&!*$!&*%%$$retVal LF@$!&*%%&!*$!&*%%$$_i\n");
    printf("JUMP chr_totaly_the_end_no_kidding\n");
    printf("\n");
    printf("#error box\n");
    printf("LABEL chr_only_one_error_ohno\n");
    printf("MOVE LF@$!&*%%&!*$!&*%%$$ErrorVal int@1\n");
    printf("MOVE LF@$!&*%%&!*$!&*%%$$retVal int@0\n");
    printf("JUMP chr_totaly_the_end_no_kidding\n");
    printf("\n");
    printf("#the absolute end, whatever happened\n");
    printf("LABEL chr_totaly_the_end_no_kidding\n");
    printf("PUSHS LF@$!&*%%&!*$!&*%%$$retVal\n");
    printf("PUSHS LF@$!&*%%&!*$!&*%%$$ErrorVal\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
}

int generate(trAK instr){
    printf("%s %s %s %s\n",instr.name,instr.boku ? instr.boku : "",instr.no ? instr.no : "",instr.pico ? instr.pico : "");
    return SUCCESS;
}