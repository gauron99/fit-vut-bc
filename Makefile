
.PHONY: all simple-one-unit-big-boss

MAIN=secret.cpp
OUT_BIN=secret

all: cl build-in-c
	
build-in-c: $(MAIN_C)
	gcc -std=c99 $(MAIN_C).c -o $(MAIN_C) -g

rs:
	sudo ./secret -l

r:
	sudo ./secret -s adminboi

cl:
	rm -f secret *.o



