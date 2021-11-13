
.PHONY: secret all clean l clean-o archive readme

CC=gcc
CFLAGS= -std=gnu99 -g -Wall 
LDFLAGS= -lcrypto -lssl -lpcap

MAIN=secret

all: clean $(MAIN) clean-o

$(MAIN): $(MAIN).o server.o client.o
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN).o server.o client.o $(LDFLAGS)

$(MAIN).o: $(MAIN).c $(MAIN).h
	$(CC) $(CFLAGS) -c $<

server.o: server.c $(MAIN).h
	$(CC) $(CFLAGS) -c $< $(LDFLAGS)

client.o: client.c $(MAIN).h
	$(CC) $(CFLAGS) -c $< $(LDFLAGS)

l:
	sudo ./secret -l

clean:
	rm -f secret *.o vgcore* 

clean-o:
	rm -f *.o

archive:
	tar -czvf xfridr08.tar.gz *.c *.h Makefile $(MAIN).1 manual.pdf

readme:
	man -l $(MAIN).1