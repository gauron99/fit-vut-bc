.PHONY: all clean build

CC=gcc
CFLAGS= -g -std=c99 -Wall
MAIN=main

all: clean build

build: $(MAIN).c
	$(CC) $< $(CFLAGS) -o $(MAIN)

# tady pridat standardni parametry pro spusteni
runbasic: 
	./$(MAIN) --vic_aut 
run:
	./$(MAIN) -b 165 --vic_aut
malo:
	./$(MAIN) -b 20,5 --pulpaleta 11,5 --paleta 7,3  --dvojpaleta 0 --vic_aut 


clean:
	rm -f *.o $(MAIN)
