.PHONY: all clean build

CC=gcc
CFLAGS= -g -std=c99 -Wall
MAIN=main

all: clean build

build: $(MAIN).c
	$(CC) $< $(CFLAGS) -o $(MAIN)

# tady pridat standardni parametry pro spusteni
run: 
	./$(MAIN) --vic_aut 
runbaliky:
	./$(MAIN) -b 165 --vic_aut
test1:
	./$(MAIN) -b 20,5 --pulpaleta 11,5 --paleta 7,3  --dvojpaleta 5,2 --vic_aut 


clean:
	rm -f *.o $(MAIN)
