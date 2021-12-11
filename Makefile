.PHONY: all clean build

CC=gcc
CFLAGS= -g -std=c99 -Wall
MAIN=main

all: clean build

build: $(MAIN).c
	$(CC) $< $(CFLAGS) -o $(MAIN)

# tady pridat standardni parametry pro spusteni
run: 
	./$(MAIN) -b 20,5 --pulpaleta 16,5 --paleta 7,3  --dvojpaleta 3,3 --vic_aut 
malo:
	./$(MAIN) -b 15 --pulpaleta 10 --paleta 10  --dvojpaleta 2


clean:
	rm -f *.o $(MAIN)
