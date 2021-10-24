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
// #define _XOPEN_SOURCE 700 

#define PACKET_MAX_SIZE 1500

// #define __FAVOR_BSD          // important for tcphdr structure

#include <stdio.h>
#include <stdlib.h> 					// exit()
#include <string.h>						// memset, memcpy

#include <netdb.h> //1
#include <unistd.h> 					// close()


#include <netinet/ip.h> 			// struct iphdr (+server)
#include <netinet/ip_icmp.h>	//icmphdr, ICMP_ECHO


//pcap server
// #include <sys/types.h>

#include <pcap/pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/if_ether.h> 
#include <arpa/inet.h>

#include <openssl/aes.h> 			// encryption




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

// ------------------ CHECKSUM STUFF ------------------ //

// unsigned short checksum(void *b, int len)
// {	unsigned short *buf = b;
// 	unsigned int sum=0;
// 	unsigned short result;

// 	for ( sum = 0; len > 1; len -= 2 )
// 		sum += *buf++;
// 	if ( len == 1 )
// 		sum += *(unsigned char*)buf;
// 	sum = (sum >> 16) + (sum & 0xFFFF);
// 	sum += (sum >> 16);
// 	result = ~sum;
// 	return result;
// }


//http://www.faqs.org/rfcs/rfc1071.html
unsigned short checksum(void *b,int l){
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

// ------------------ SERVER STUFF ------------------ //

void p(char *s){
	printf("%s\n",s);
}

void server() {
	char errbuf[PCAP_ERRBUF_SIZE];  // constant defined in pcap.h
  pcap_t *handle;                 // packet capture handle 
  pcap_if_t *alldev, *dev ;       // a list of all input devices
  char *devname;                  // a name of the device
  struct in_addr a,b;
  bpf_u_int32 netaddr;            // network address configured at the input device
  bpf_u_int32 mask;               // network mask of the input device
  struct bpf_program fp;          // the compiled filter


	// open the input devices (interfaces) to sniff data
  if (pcap_findalldevs(&alldev, errbuf))
    printErr("Can't open input device(s)");

  // list the available input devices
  printf("Available input devices are: ");
  for (dev = alldev; dev != NULL; dev = dev->next){
    printf("%s ",dev->name);
  }
  printf("\n");

  devname = alldev->name;  // select the name of first interface (default) for sniffing 
  

}

// ------------------ CLIENT STUFF ------------------ //

void client(char *file, char *host){
	if(file == "" || host == "" || file == NULL || host == NULL){
		printf("-r(file): %s\n",file);
		printf("-s(host): %s\n",host);
		// printErr("Not all necessary parameters were given."); //TODO uncomment later
	}

	int fd; //file descriptor for socket
	struct addrinfo *target,*tmp,hints;

	memset(&hints,0,sizeof(hints));

	hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = 0;

	p("hostis:");
	printf("%s\n",host);

// gethostbyname is obsolete, use -> getaddrinfo()
	int tmpRet = getaddrinfo(host,NULL,&hints,&tmp);
	if(tmpRet != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(tmpRet));
		exit(1);
	}

	// snippet taken from https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
	// getaddrinfo returns in 'pai' (last argument) a list of address structures
	// try to connect to atleast one.
	int protocol;
	for(target=tmp; target != NULL;target=target->ai_next){
		protocol = target->ai_family == AF_INET ? IPPROTO_ICMP : IPPROTO_ICMPV6;

		printf("---\n");
		printf("addr:%d\n",target->ai_addr->sa_family);
		printf("family:%d\n",target->ai_family);
		printf("sock:%d\n",target->ai_socktype);
		
		fd = socket(target->ai_family,target->ai_socktype,protocol);
		printf("fd:%d\n",fd);
		if (fd < 0){
			printf("chyba?\n");
			continue;
		}
		//if socket is successfully created, break with valid struct in 'target'
		break;
	}
	freeaddrinfo(tmp);

	if(target == NULL){
		printErr("No socket was created successfully, maybe you forgot sudo?\n");
	}

	//init packet -- largest possible packet size is set to 1500B
	char packet[PACKET_MAX_SIZE];
	memset(&packet,0,PACKET_MAX_SIZE);

	char data[] = "yello boy how you been\n";


	/**
	 * PACKET
	 * [0-7] -- icmp header
	 * [8-1500] -- data
	 * 
	 * 
	*/

	// ICMP header length is 8 bytes
	struct icmphdr *icmpH = (struct icmphdr*)packet;
	icmpH->type = ICMP_ECHO;
	icmpH->checksum = 0;

	//copy the data to packet after icmp header
	memcpy(packet+sizeof(struct icmphdr),data,sizeof(data));

	//calculate checksum
	icmpH->checksum = checksum(packet,sizeof(struct icmphdr)+sizeof(data));
	
	//current possible data len. Maybe if some special info is sent in the first
	//packet of total len of file being send or something
	unsigned int maxDataLen = PACKET_MAX_SIZE-sizeof(icmpH); 

	// https://man7.org/linux/man-pages/man3/sendto.3p.html
	if (sendto(fd,packet,sizeof(struct icmphdr)+sizeof(data),0,(struct sockaddr*)target->ai_addr,target->ai_addrlen) < 0){
		printErr("Function sendto failed, exiting.\n");
	}

	//encryption


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