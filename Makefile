.PHONY: all clean build

CC=gcc
CFLAGS= -g -std=c99 -Wall 
MAIN=main

all: clean build

build: $(MAIN).c
	$(CC) $< $(CFLAGS) -o $(MAIN)

# tady pridat standardni parametry pro spusteni
run: 
	./$(MAIN) -b 10 --paleta 5

clean:
	rm -f *.o $(MAIN)