CFLAGS = -g
all: ifj20 clean

ifj20: parser.o generator.o symtable.o prectableandrules.o precanalysis.o scanner.o custom_string.o ifj20.o
	gcc $(CFLAGS) symtable.o generator.o parser.o scanner.o precanalysis.o prectableandrules.o custom_string.o ifj20.o -o ifj20

clean:
	rm -f *.o
