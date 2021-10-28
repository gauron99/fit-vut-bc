// secret(c) -- main file

/*
~~~~~~~~~ TODO TODO TODO TODO ~~~~~~~~~

-- resolve sending a file
--- resolve multiple file sending
- resolve client IPV6 --> cant ping ::1
---- encrypt a file

- unalloc: *file, *host, *filebuff
*/


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
 * #1:
 * this is defined because of addrinfo struct in netdb.h (check the definition)
 * it is defined under a condition a XOPEN2K POSIX is defined, which is fairly
 * common in g++ or using -std=gnuXX but not in gcc -std=cXX, therefore
*/
/**
 * #2:
 * by defining XOPEN_SOURCE compatibility with pcap.h doesn't work because
 * of the unsigned chars/ints defined in sys/types.h (there is a #define
 * limitation for __USE_MISC which isn't met, therefore I'm using standard gcc)
*/
// #define _XOPEN_SOURCE 700 

//maximum size of a packet
#define PACKET_MAX_SIZE 1500

#include <stdio.h>
#include <stdlib.h> 					// exit()
#include <string.h>						// memset, memcpy

#include <netdb.h>
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

//file
#include <sys/stat.h>

// encryption
#include <openssl/aes.h>


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


void
fileExists(char *file){
	if( access(file,F_OK | R_OK) != 0) {
		printErr("File doens't exist and/or has wrong read permissions set");
	}
}

char *fileOpenReadBytes(char *in, long int *len){
	char* ret;

	// code snippet to open file in binary mode
	//https://stackoverflow.com/a/22059317
	FILE *f = fopen(in,"rb"); //open in binary mode
	fseek(f,0,SEEK_END);
	*len = ftell(f);
	rewind(f);
	
	ret = malloc(*len);
	if(!ret){
		free(in);
		printErr("malloc failed");
	}
	fread(ret,*len,1,f);
	fclose(f);

	return ret;
}

// ------------------------------- PARSER FUNC------------------------------- //

int
parser(int argc, char **argv, char **file,char **host){
  int param_s=0;	//host
  int param_r=0; 	//file
  int param_l=0;	//is_server?

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
    else if(!strcmp(argv[i],"-s")){ //host         
      if(argv[i+1] != NULL){
        *host = malloc(sizeof(strlen(argv[i+1])));
        *host = argv[++i];
        param_s=1;
      } else {
        printErr("Parameter for -s was not provided");
      }
    }
    else if(!strcmp(argv[i],"-r")){ //file
      if(argv[i+1] != NULL){
				fileExists(argv[i+1]);
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

// ------------------ CHECKSUM FUNC ------------------ //

// this function is taken from:
// http://www.faqs.org/rfcs/rfc1071.html
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


void p(char *s){
	printf("%s\n",s);
}

/**
 * handle captured packets here
 * this is a pcap_loop() callback function
 * 
 * inspiration from:
 * https://www.binarytides.com/packet-sniffer-code-c-libpcap-linux-sockets/
 * https://www.binarytides.com/packet-sniffer-code-c-linux/
*/
void packet_hdlr_cb(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){

	//size of the packet
	int size = header->len;
	static unsigned int seq_num = 0;

	printf("%d: another one! (len: %d)\n",++seq_num,size);

	/**
	 * from ethernet header, get 'type' --> IPv4 / IPv6
	 * then resolve IP header (IPv4 == 20 bytes) --> any useful info in here?
	 * and finally ICMP
	*/



	//Get the IP Header part of this packet , excluding the ethernet header
	struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));
	// int size_iph = iph


	switch (iph->protocol){ //Check the Protocol and do accordingly...
		case 1: // ICMP
	}
}



// ------------------ SERVER FUNC ------------------ //

/*
 * inspiration from:
 * 'sniff-filter.c' file in "isa/soubory" from 'Petr Matousek, 2020'
*/
void server() {
	char errbuf[PCAP_ERRBUF_SIZE];  // constant defined in pcap.h
  pcap_t *handler;                 // packet capture handle 
  pcap_if_t *alldev, *dev ;       // all available devices

  char *devname = NULL;           // a name of the chosen device
  struct in_addr a,b;

  bpf_u_int32 netaddr;            // network address configured at the input device
  bpf_u_int32 mask;               // network mask of the input device
  
	struct bpf_program filter;          // the compiled filter


	// open the input devices (interfaces) to choose from
  if (pcap_findalldevs(&alldev, errbuf))
    printErr("Can't open input device(s)");

  // list the available input devices
  for (dev = alldev; dev != NULL; dev = dev->next){
		if(!strcmp(dev->name,"any")){
		  devname = dev->name;
			break;
		}
  }
	//set first one (default) if "any" is not available
	if(devname == NULL){
		devname = alldev->name;
	}

	printf("devname:%s\n",devname);
  
	// get IP address and mask of the sniffing interface
  if (pcap_lookupnet(devname,&netaddr,&mask,errbuf) == -1)
    printErr("Can't get IP & mask of chosen interface");


/* TODO
possibly dont even ahve to use devname, therefore  looking for interfaces, just
use NULL or "any" right here for all devices to be sniffed, because it is
unknown what devices are the packets coming from
*/

	memset(errbuf,0,PCAP_ERRBUF_SIZE);

	//open the interface
	if((handler = pcap_open_live(devname,PACKET_MAX_SIZE,1,1000,errbuf)) == NULL){
		fprintf(stderr,"Error: pcap_open_live(): %s\n",errbuf);
		exit(1);
	}

	if(strlen(errbuf) != 0){
		printf("Warning from pcap_open_live: %s\n",errbuf);
	}

	//add custom filter & apply
	if(pcap_compile(handler,&filter,"icmp or icmp6",0,netaddr) == -1){
		printErr("Can't compile given filter");
	}


	if(pcap_setfilter(handler,&filter) == -1){
		printErr("Can't set filter for chosen device");
	}

	printf("Starting server, press CTRL-C to stop me!\n");

	//begin sniffing -- cnt is '-1' --> catch packets until interrupted
	if(pcap_loop(handler,-1,packet_hdlr_cb,NULL) == -1){
		printErr("Can't sniff, pcap_loop failed");
	}

	// capturing stopped, deallocate vars
  pcap_close(handler);
  pcap_freealldevs(alldev);
}

// ------------------ CLIENT FUNC ------------------ //

void client(char *file, char *host){
	if(file == "" || host == "" || file == NULL || host == NULL){
		printf("-r(file): %s\n",file);
		printf("-s(host): %s\n",host);
		printErr("Not all necessary parameters were given."); //TODO uncomment later
	}

	long int filelen;
	char *filebuff = fileOpenReadBytes(file,&filelen);

	int fd; //file descriptor for socket
	struct addrinfo *target,*tmp,hints;

	memset(&hints,0,sizeof(hints));

	hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
	hints.ai_socktype = SOCK_RAW;

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
		printf("addr:%s\n",target->ai_addr->sa_family==2?"ipv4":"ipv6 or other");
		printf("family:%s\n",target->ai_family==2?"ipv4":"icmp6 or other");
		printf("protocol:%s\n",protocol == 1 ? "ICMP" : "ICMP6 maybe?");
		
		fd = socket(target->ai_family,target->ai_socktype,protocol); 
		if (fd < 0){
			close(fd);
			continue;
		}
		//if socket is successfully created, break with valid struct in 'target'
		break;
	}


	// save these values so we can free the memory
	struct sockaddr servinfo = *(struct sockaddr*)target->ai_addr;
	socklen_t servlen = target->ai_addrlen;
	
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
	 * 
	 * [8-1500] -- data
	 * 
	 * 
	*/

	// ICMP header length is 8 bytes

	struct icmphdr *icmpH = (struct icmphdr*)packet;
	icmpH->code = ICMP_ECHO;
	icmpH->checksum = 0;

	int datalen = sizeof(data);
	// cut down size to available space if its larger than PACKET_MAX_SIZE
	int available = PACKET_MAX_SIZE-sizeof(struct icmphdr);

	// get file length in bytes (5 bytes -- max value -> 1,099,511,627,775 this
	// should be enough for the whole file)
	char info[5];
	sprintf(info,"%d",filelen);


	//copy the data to packet after icmp header
	memcpy(packet+sizeof(struct icmphdr),data,datalen);

	//calculate checksum
	if(protocol==IPPROTO_ICMP){
		icmpH->checksum = checksum(packet,sizeof(struct icmphdr)+datalen);
	}

	// https://man7.org/linux/man-pages/man3/sendto.3p.html

	if (sendto(fd,packet,sizeof(struct icmphdr)+datalen,0,&servinfo,servlen) < 0){
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
		server();
	} else{ //IS_CLIENT
		client(file,host);
		// free(file);
		// free(host);
	}

	

	return 0;
}


// up is hard to reach