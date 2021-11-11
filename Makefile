


.PHONY: secret all cl build l local

CC=gcc
CFLAGS= -std=gnu99 -g -Wall 
LDFLAGS= -lcrypto -lssl -lpcap

MAIN=secret

all: clean $(MAIN) clear-o

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

clear-o:
	rm *.o

local:
	sudo ./secret -s localhost -r file_to_send.txt

local-big:
	sudo ./secret -s localhost -r file_to_send_larger.txt
