#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BASIC='\033[0m'
ORANGE='\033[0;33m'

echo -------------------------- COMPILER TESTS --------------------------
echo

echo ====================================================================
echo - - - [TESTS SET 1] - Lexical Analysis tests - - - - - - - - - - - -
echo These input source codes should pass lexical analysis with no error
echo
let i=0
let LEXpassed=0
let LEXwarning=0
let LEXfailed=0
let LEXtotal=0
let SEMpassed=0
let SEMwarning=0
let SEMfailed=0
let SEMtotal=0
lexTests1=`ls tests/lexPASS/*.go`

for eachfile in $lexTests1
do
    i=$(($i + 1))
    LEXtotal=$(($LEXtotal + 1))
    echo -e ${BASIC}[SET 1: Test $i]

    result=$(../ifj20 <../tests/$eachfile)
    return_value=$?

	if [ "$return_value" -eq "0" ]; then
	    echo -e ${GREEN}OK  - ${eachfile}
        LEXpassed=$((LEXpassed+1))
    elif [ "$return_value" -eq "1" ]; then
        LEXfailed=$((LEXfailed+1))
	    echo -e ${RED}ERROR  - ${eachfile} - return value expected 0, returned $return_value instead
	else
	    echo -e ${ORANGE}OK  - ${eachfile} - lexically ok, return value $return_value
        LEXwarning=$((LEXwarning+1))
	fi
done

echo
echo -e ${BASIC}====================================================================
echo - - - [TESTS SET 2] - Lexical Analysis tests - - - - - - - - - - - -
echo These input source codes should pass lexical analysis with error code 1
echo

let j=0
lexTests2=`ls ../tests/tests/lexERR/*.go`
for eachfile in $lexTests2
do
    j=$(($j + 1))
    LEXtotal=$(($LEXtotal + 1))
    echo -e ${BASIC}[SET 2: Test $j]

    result=$(../ifj20 <../tests/$eachfile)
    return_value=$?

	if [ "$return_value" -eq "1" ]; then
	    echo -e ${GREEN}OK  - ${eachfile}
        LEXpassed=$((LEXpassed+1))
	elif [ "$return_value" -eq "0" ]; then
	    echo -e ${RED}ERROR  - ${eachfile} - return value expected 1, returned $return_value instead
        LEXfailed=$((LEXfailed+1))
    else
        echo -e ${ORANGE}ERROR  - ${eachfile} - Lexically ok, error code is $return_value tho
        LEXwarning=$((LEXwarning+1))
	fi
done

echo -e ${BASIC}====================================================================
echo -e ${BASIC}=========================LEX=SUMMARY================================
echo -e ${BASIC}Total number of tests: ${LEXtotal}
echo -e ${GREEN}PASSED: ${LEXpassed} tests
echo -e ${ORANGE}WARNINGS: ${LEXwarning} tests - possible segfault
echo -e ${RED}FAILED: ${LEXfailed} tests
echo -e ${BASIC}====================================================================

echo ====================================================================
echo - - - [TESTS SET 1] - Semantic Analysis tests - - - - -- - - - - - -
echo These input source codes should pass lexical analysis with no error.
echo  

let k=0
semTests1=`ls ../tests/tests/semPASS/*.go`
for eachfile in $semTests1
do
    k=$(($k + 1))
    SEMtotal=$(($SEMtotal + 1))
    echo -e ${BASIC}[SET 1: Test $k]

    result=$(../ifj20 <../tests/$eachfile)
    return_value=$?

	if [ "$return_value" -eq "0" ]; then
	    echo -e ${GREEN}OK  - ${eachfile}
        SEMpassed=$((SEMpassed+1))  
    elif [ "$return_value" -eq "3" ]; then
        echo -e ${RED}ERROR  - ${eachfile} - unexpected return value $return_value
        SEMfailed=$((SEMfailed+1)) 
    elif [ "$return_value" -eq "4" ]; then
        echo -e ${RED}ERROR  - ${eachfile} - unexpected return value $return_value
        SEMfailed=$((SEMfailed+1)) 
    elif [ "$return_value" -eq "5" ]; then
        echo -e ${RED}ERROR  - ${eachfile} - unexpected return value $return_value
        SEMfailed=$((SEMfailed+1)) 
    elif [ "$return_value" -eq "6" ]; then
        echo -e ${RED}ERROR  - ${eachfile} - unexpected return value $return_value
        SEMfailed=$((SEMfailed+1)) 
    elif [ "$return_value" -eq "7" ]; then
        echo -e ${RED}ERROR  - ${eachfile} - unexpected return value $return_value
        SEMfailed=$((SEMfailed+1)) 
    elif [ "$return_value" -eq "9" ]; then
        echo -e ${RED}ERROR  - ${eachfile} - unexpected return value $return_value
        SEMfailed=$((SEMfailed+1)) 
	else
	    echo -e ${ORANGE}ERROR  - ${eachfile} - semantically ok, return value $return_value
        SEMwarning=$((SEMwarning+1))
	fi
done


echo
echo -e ${BASIC}====================================================================
echo - - - [TESTS SET 2] - Semantic Analysis tests - - - - - - - - - - - -
echo These input source codes should not pass Semantic analysis
echo

let l=0
semTests2=`ls ../tests/tests/semERR/*.go` 
for eachfile in $semTests2
do
    l=$(($l + 1))
    SEMtotal=$(($SEMtotal + 1))
    echo -e ${BASIC}[SET 2: Test $l]

    result=$(../ifj20 <../tests/$eachfile)
    return_value=$?

    #3,4,5,6,7,9 su ok errory
	if [ "$return_value" -eq "3" ]; then
	    echo -e ${GREEN}OK  - ${eachfile}
        SEMpassed=$((SEMpassed+1))
    elif [ "$return_value" -eq "4" ]; then
        echo -e ${GREEN}OK  - ${eachfile}
        SEMpassed=$((SEMpassed+1)) 
    elif [ "$return_value" -eq "5" ]; then
        echo -e ${GREEN}OK  - ${eachfile}
        SEMpassed=$((SEMpassed+1)) 
    elif [ "$return_value" -eq "6" ]; then
        echo -e ${GREEN}OK  - ${eachfile}
        SEMpassed=$((SEMpassed+1)) 
    elif [ "$return_value" -eq "7" ]; then
        echo -e ${GREEN}OK  - ${eachfile}
        SEMpassed=$((SEMpassed+1)) 
    elif [ "$return_value" -eq "9" ]; then
        echo -e ${GREEN}OK  - ${eachfile}
        SEMpassed=$((SEMpassed+1)) 
    elif [ "$return_value" -eq "0" ]; then
        echo -e ${RED}ERROR  - ${eachfile} - passed successfully, expected error
        SEMfailed=$((SEMfailed+1))            
	else
	  echo -e ${ORANGE}ERROR  - ${eachfile} - unexpected return value $return_value
      SEMwarning=$((SEMwarning+1))
	fi
done


echo -e ${BASIC}====================================================================
echo -e ${BASIC}=========================SEM=SUMMARY================================
echo -e ${BASIC}Total number of tests: ${SEMtotal}
echo -e ${GREEN}PASSED: ${SEMpassed} tests
echo -e ${ORANGE}WARNINGS: ${SEMwarning} tests - possible segfault etc
echo -e ${RED}FAILED: ${SEMfailed} tests
echo -e ${BASIC}====================================================================
