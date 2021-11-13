// secret(c) -- main file

////////////////////////////////////////////////////////////////////////////////
//  Used for ISA project @BUT_FIT (cz:FIT VUT)                                //
//  author: David Fridrich                                                    //
//  file: secret.c - implementation of functions to transfer file through icmp//
//  last updated: 05.11.2021                                                  //
////////////////////////////////////////////////////////////////////////////////


// SPUSTENI
// secret -r <file> -s <ip|hostname> [-l]
// -r <file> : specifikace souboru pro přenos
// -s <ip|hostname> : ip adresa/hostname na kterou se má soubor zaslat
// -l : pokud je program spuštěn s tímto parametrem, jedná se o server,
// 	který naslouchá příchozím ICMP zprávám a ukládá soubor do stejného adresáře,
//  kde byl spuštěn.


/**
 * #1:
 * this is defined because of addrinfo struct in netdb.h (check the definition)
 * it is defined under a condition a XOPEN2K POSIX is defined, which is fairly
 * common in g++ or using -std=gnuXX but not in gcc -std=cXX, therefore we would
 * want to use it BUT #2

 * #2:
 * Defining XOPEN_SOURCE doesn't work, because compatibility with pcap.h doesn't
 * work because of the unsigned chars/ints defined in sys/types.h 
 * (there is a #define limitation for __USE_MISC which isn't met, therefore 
 * I'm using standard gcc)
*/
// #define _XOPEN_SOURCE 700


// struct settings *ptr;
struct settings *ptr;
const char *encryptionKey = "xfridr08";

// more on this: https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
#define verbose_print(fmt, ...) \
	do { if (ptr->_verbose_printing) printf(""fmt, __VA_ARGS__); } while(0)


#include "secret.h" // mine

enum who_am_i {
	IS_SERVER,
	IS_CLIENT,
};


// ########################################################################## //
//																functions																		//


void init(){
	ptr = malloc(sizeof(settings));
	if(!ptr){
		printErr("Malloc failed @init()");
	}
	ptr->verbose_printing = 0;
	ptr->file_name 				= NULL;
	ptr->filebuff					= NULL;
}

void printErr(char *s){
	cleanStruct();
	fprintf(stderr,"Error: %s\n",s);
	exit(1);
}

void cleanStruct(){
	if(ptr != NULL){
		if(ptr->f != NULL){
			fclose(ptr->f);
		}
		if(ptr->file_name != NULL){
			free(ptr->file_name);
		}
		if(ptr->filebuff!= NULL){
			free(ptr->filebuff);
		}
		free(ptr);
	}
}

void printHelp(){
	printf("This is a help message for secret\n");
	printf("job - Send/recieve a crypted file through ICMP traffic\n");
	printf("----- ----- ----- ----- -----\n");
	printf("Note: Program must be run with sudo\n");
	printf("USAGE\n"); 
	printf("	./secret [parameters]\n");
	printf("PARAMETERS\n");
	printf("I) for server\n");
	printf("	[-l]  start a server and listen for ICMP messages\n");
	printf("		-- note: if [-l] is given, no client options can be given\n");
	printf("II) for client\n");
	printf("	[-s]  <ip|hostname> of server (where to send)\n");
	printf("	[-r]  <file> to send\n");
	printf("III) for both\n");
	printf("	[-v]  enable verbose printing (prints out filelen_char throughout)\n");
	exit(0);
}

int fileIsEmpty(char *file){
	FILE *f = fopen(file,"r");
	if(f == NULL){
		printErr("Failed to open a file[server-side]");
	}
	char c = getc(f);

	if (c == EOF){
		fclose(f);
		return 1;

	} else {
		ungetc(c,f);
		fclose(f);
		return 0;
	}
}

int fileExists(char *file){
	if(access(file,F_OK | R_OK) != 0) {
		return 0;	
	}
	return 1;
}

char *getFilenameFromPath(char *path){

// find last backslash
	int backslashPos = -1;
	const int l = strlen(path);
	for (int i = l-1; i >= 0; --i){
		if (path[i] == '/'){
			backslashPos = i;
			break;
		}
	}
	if(backslashPos < 0){
		return path;

	} else {

	char *p = malloc(l-backslashPos);
	if(!p){
		printErr("Malloced failed @getFilenameFromPath()");
	}
	memcpy(p,path+backslashPos+1,l-backslashPos);
		return p;
	}
}

// ------------------------------- PARSER FUNC------------------------------- //

int
parser(int argc, char **argv, char **file,char **host){
	if(argc == 1){
		printHelp();
	}

	int is_server = 0;	

  for (int i = 1; i < argc; i++){
    if(!strcmp(argv[i],"-h")||!strcmp(argv[i],"--help")){
      printHelp();
    }
    else if(!strcmp(argv[i],"-l")){ //is server
      is_server = 1;
    }
    else if(!strcmp(argv[i],"-s")){ //host
      if(argv[++i] != NULL){
        *host = malloc(strlen(argv[i])+1);
				if(*host == NULL){
					printErr("Malloc failed @parser(), for *host");
				}
				memcpy(*host,argv[i],strlen(argv[i])+1);
      } else {
        printErr("Parameter for -s (host) was not provided");
      }
    }
    else if(!strcmp(argv[i],"-r")){ //file
      if(argv[++i] != NULL){
				if(!fileExists(argv[i])){
					printErr("File doesn't exist and/or has wrong read permissions set");
				}
        *file = malloc(strlen(argv[i])+1);
				if(*file == NULL){
					printErr("Malloc failed @parser() for *file");
				}
        memcpy(*file,argv[i],strlen(argv[i])+1);
				
      } else {
        printErr("Parameter for -r (file) was not provided");
      }
    }
		else if(!strcmp(argv[i],"-v")){//verbose printing
			ptr->verbose_printing = 1;
		} 
		
		else {
			if(*host!=NULL){free(*host);}
			if(*file!=NULL){free(*file);}
      printErr("Unknown parameter was given");
    }
  }

  // check parameters

  if(is_server && (*file != NULL || *host != NULL)){
    printErr("Parameter -l is exclusive. It can't be mixed with parameters -r & -s");
  }

	return is_server ? IS_SERVER : IS_CLIENT;
}

// ------------------ CHECKSUM FUNC ------------------ //

// source:
// http://www.faqs.org/rfcs/rfc1071.html
unsigned short checksum(void *b,int l,int p){

	if(p == IPPROTO_ICMP){

		unsigned short *addr = b;
		unsigned int sum = 0;

		//inner loop
		for (; l > 1; l-=2){
			sum += *addr++;
		}

		//if there is one byte left over (odd count)
		if(l==1){
			sum+= *addr;
		}

		//fold 32-bit sum to 16 bits
		while(sum>>16){
			sum = (sum & 0xffff) + (sum >> 16);
		}
		return (~sum);

	}
	return 0;
}

void sigint_handler(int sn){
	cleanStruct();
	printf("CTRL+C was pressed, exiting\n");
	exit(0);
}

// --------------------------------- MAIN --------------------------------- //
int main(int argc, char **argv){

	signal(SIGINT,sigint_handler);

	//initialize some values
	char *file = NULL; // r option
	char *host = NULL; // s option

	init();
	
	// parse cmli arguments & decide if to startup client or server
	if(parser(argc,argv, &file, &host) == IS_SERVER){
		server();
	} else{ //IS_CLIENT

		client(file,host);
		free(file);
		free(host);
	}
	return 0;
}


// up is hard to reach