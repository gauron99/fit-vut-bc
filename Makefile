
.PHONY: all rs r cl build-in-c

CC=gcc
MAIN=secret

all: cl build
	
build: $(MAIN).c $(MAIN).h
	$(CC) -std=gnu99 $(MAIN).c -o $(MAIN) -g -lpcap

build-easy: $(MAIN).c $(MAIN).h
	$(CC) $(MAIN).c -o $(MAIN) -g -lpcap


rs:
	sudo ./secret -l

r:
	sudo ./secret -s adminboi

cl:
	rm -f secret *.o vgcore*

