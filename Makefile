all: ifj20 clean

ifj20: parser.o prectableandrules.o precanalysis.o scanner.o custom_string.o ifj20.o
	gcc parser.o scanner.o precanalysis.o prectableandrules.o custom_string.o ifj20.o -o ifj20

ifj20.o: ifj20.c ifj20.h

scanner.o: scanner.c scanner.h

parser.o: parser.c parser.h

custom_string.o: custom_string.c custom_string.h

precanalysis.o: precanalysis.c precanalysis.h

prectableandrules.o: prectableandrules.c

clean:
	rm -f *.o
