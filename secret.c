// secret(c) -- main file
/*
*************************************************
 *                                  						*
 * ISA project 2021 - secret ICMP file transfer *
 * Author: David Fridrich           						*
 * last mod: 19.10.2021													*
 *                                  						*
*************************************************
*/

// SPUSTENI
// secret -r <file> -s <ip|hostname> [-l]
// -r <file> : specifikace souboru pro přenos
// -s <ip|hostname> : ip adresa/hostname na kterou se má soubor zaslat
// -l : pokud je program spuštěn s tímto parametrem, jedná se o server,
// 	který naslouchá příchozím ICMP zprávám a ukládá soubor do stejného adresáře,
//  kde byl spuštěn.


/**
 * this is defined because of addrinfo struct in netdb.h (check the definition)
 * it is defined under a condition a XOPEN2K POSIX is defined, which is fairly
 * common in g++ or using -std=gnuXX but not in gcc -std=cXX, therefore
 */
#define _XOPEN_SOURCE 700 

#define PACKET_MAX_SIZE 1500

#include <stdio.h>
#include <stdlib.h> //exit
#include <string.h>

#include <netdb.h> //1

#include <unistd.h>

#include <sys/stat.h>
#include <sys/socket.h> //2
#include <sys/types.h> // 3 -- aready in netdb.h

// #include <netinet/in_systm.h> // ?
// #include <netinet/in.h>

#include <netinet/ip.h> //struct iphdr
#include <netinet/ip_icmp.h>

#include <arpa/inet.h> //inet_addr()


enum who_is {
	IS_SERVER,
	IS_CLIENT,
};

char *encryptionKey = "xfridr08";

void printErr(char *s){
	fprintf(stderr,"Error: %s\n",s);
	exit(1);
}

// help function prints out help message
void printHelp(){
	printf("This is a help message for secret\n");
	printf("job - Send/recieve a crypted file through ICMP traffic\n");
	printf("----- ----- ----- ----- -----\n");
	printf("USAGE\n"); 
	printf("	./secret [parameters]\n");
	printf("PARAMETERS\n");
	printf("I) for server\n");
	printf("	[-l]  start a server and listen for ICMP messages\n");
	printf("		-- is exclusive(if -l is given, nothing else can be given)\n");
	printf("II) for client\n");
	printf("	[-s]  <ip|hostname> of server (where to send)\n");
	printf("	[-r]  <file> to send\n");
	exit(0);
}

// ------------------------------- PARSER ------------------------------- //

int
parser(int argc, char **argv, char **file,char **host){
  int param_s=0;
  int param_r=0;
  int param_l=0;

	if(argc == 1){
		printHelp();
	}

	/*
   * parameter -l is exclusive.
	 *  if -l option is given, no other parameters can be given.
	*/
  for (int i = 1; i < argc; i++){
    if(!strcmp(argv[i],"-h")){
      printHelp();
    }
    else if(!strcmp(argv[i],"-l")){
      param_l=1;
    }
    else if(!strcmp(argv[i],"-s")){ //ip/hostname         
      if(argv[i+1] != NULL){
        *host = malloc(sizeof(strlen(argv[i+1])));
        *host = argv[++i];
        param_s=1;
      } else {
        printErr("Parameter for -s was not provided");
      }
    }
    else if(!strcmp(argv[i],"-r")){ //soubor
      if(argv[i+1] != NULL){
        *file = malloc(sizeof(strlen(argv[i+1])));
        *file = argv[++i];
        param_r=1;
      } else {
        printErr("Parameter for -r was not provided");
      }
    } else {
      printErr("Unknown parameter was given");
    }
  }

  // check parameters
  if(param_l && (param_r || param_s)){
    printErr("Parameter -l is exclusive. It can't be mixed with other parameters");
  }

	return param_l ? IS_SERVER : IS_CLIENT;
}


// ------------------ SERVER STUFF ------------------ //

void server() {

	// create a socket
	//superuser for SOCK_RAW use
	printf("> creating a socket...");
	int fd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP); 
	if(fd == -1){
		printErr("Error occured during socket setup [server]");
		close(fd);
	}
	printf("done\n");

	struct sockaddr_in serv_addr;

	//fill with zeroes
	memset(&serv_addr,0,sizeof(serv_addr));

	// serv_addr.sin_port = htons(PORT);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY; //current host IP

	printf("> binding the socket to address & port...\n");
	// bind the socket to PORT
	if( bind(fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) == -1) {
		printErr("Bind unsuccessful.");
	} 
	printf("done\n");

	// printf("> setting socket to passive mode with listen...";
	// if( listen(fd,3) < 0){
	// 	printErr("Listen unsuccessful");
	// }
	// printf("done");

	printf("starting server...Press CTRL-C to stop me\n");
	struct sockaddr_in cl_addr;
	socklen_t sock_size_cl = sizeof(cl_addr);
	int cl_fd;
	char *buf[2048];
	memset(&buf,0,sizeof(buf));

	/*
	 * when receiving a packet, its gonna be a whole IP packet
	 * and ICMP info must be extracted from it.
	 * 
	 * Copy of each ICMP reply is got, must be filtered.
	*/

	while(1){
		cl_fd = recvfrom(fd,buf,2048,0,(struct sockaddr*)&cl_addr,&sock_size_cl);

		// cout<< app.seq_no++ <<": " << "read " << cl_fd << " bytes");

		// IP info
		struct iphdr *ip_h = (struct iphdr*)buf;
		// printf("IP header is %d bytes.\n", ip_h->ihl*4); // this is in <ip.h> apparently
		// ICMP info 	
		struct icmphdr *icmp_h = (struct icmphdr *)((char *)ip_h + (4*(ip_h->ihl) ));
		// if(icmp_h->type == ICMP_ECHO){
			// printf("ICMP msgtype=%d, code=%d", icmp_h->type, icmp_h->code);
		// }

	}
	exit(0); //dont return control -> when server stops running, exit.
}



// ------------------ CLIENT STUFF ------------------ //

void client(char *file, char *host){
	if(file == "" || host == ""){
		printf("-r: %s\n",file);
		printf("-s: %s\n",host);
		// printErr("Not all necessary parameters were given."); //uncomment later
		printf("warning, not all parameters set\n"); //DEBUG
	}

	printf("------- Client info -------\n\n");

	struct hostent *source;
	struct sockaddr_in target_addr, source_addr;
	struct icmp *icmp; //icmp part of packet

	char source_name[256]; // for client machine name (you)

	int fd;

	struct addrinfo *target,*tmp,hints;

// get source ip adress
	if(	gethostname(source_name,sizeof(source_name)) < 0){
		printErr("Couldn't resolve hostname[client]");
	}
	printf("me: %s\n",source_name);

// gethostbyname is obsolete, use -> getaddrinfo()

// get target(destination) ip adress
	memset(&hints,0,sizeof(hints));

	hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = 0;

	int tmpRet;
	if(tmpRet = (getaddrinfo(host,NULL,&hints,&tmp)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(tmpRet));
	}

	//resolve protocol by given ip adress (as host arg)

	// snippet taken from https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
	// getaddrinfo returns in 'pai' (last argument) a list of address structures
	// try to connect to atleast one.
	int protocol;
	for(target=tmp; target != NULL;target=target->ai_next){
		protocol = target->ai_family == AF_INET ? IPPROTO_ICMP : IPPROTO_ICMPV6;
		fd = socket(target->ai_family,target->ai_socktype,protocol);
		if (fd < 0){
			continue;
		}
		//if socket is successfully created, break with valid struct in 'target'
		break;
	}
	freeaddrinfo(tmp);

	if(target == NULL){
		printErr("No socket was created successfully\n");
	}

	//init packet -- largest possible packet size is set to 1500B
	char packet[PACKET_MAX_SIZE];
	memset(&packet,0,PACKET_MAX_SIZE);

	/** TODO TODO TODO
	 * structure of the packet itself:
	 * first 2 bytes (16 bit) - [0],[1] == seq. number of packet... max val 65,536
	 * this means 1498 bytes per packet of data can be sent. (1.498kB)
	 */

	//resolve ICMP header
	struct icmphdr *icmpHdr = (struct icmphdr*)packet;
	icmpHdr->type = ICMP_ECHO;
	icmpHdr->checksum = 0; //do this for max cover & legitimity of the packet ;)

}	 


// --------------------------------- MAIN --------------------------------- //
int main(int argc, char **argv){

	//initialize some values
	char *file = NULL; // r option
	char *host = NULL; // s option


	// parse cmli arguments & decide if startup client or server
	if(parser(argc,argv, &file, &host) == IS_SERVER){
		printf("server it is\n");
		server();
	} else{ //IS_CLIENT
		printf("client it is\n");
		client(file,host);
	}

	return 0;
}


// up is hard to reach