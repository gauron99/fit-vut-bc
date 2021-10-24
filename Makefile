
.PHONY: all rs r cl build-in-c

MAIN=secret

all: cl build-in-c
	
build-in-c: $(MAIN).c $(MAIN).h
	gcc -std=gnu99 $(MAIN).c -o $(MAIN) -g -lpcap

rs:
	sudo ./secret -l

r:
	sudo ./secret -s adminboi

cl:
	rm -f secret *.o

