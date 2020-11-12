all: ifj20 clean

ifj20: scanner.o custom_string.o ifj20.o
	gcc scanner.o custom_string.o ifj20.o -o ifj20

ifj20.o: ifj20.c ifj20.h

scanner.o: scanner.c scanner.h
custom_string.o: custom_string.c custom_string.h

clean:
	rm -f *.o