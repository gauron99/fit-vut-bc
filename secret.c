// secret(c) -- main file

/*
~~~~~~~~~ TODO TODO TODO TODO ~~~~~~~~~

-- resolve sending a file
--- resolve multiple file sending
- resolve client IPV6 --> cant ping ::1
---- encrypt a file

- unalloc: *file, *host, *filebuff
*/



////////////////////////////////////////////////////////////////////////////////
//  Used for ISA project @BUT_FIT (cz:FIT VUT)                                //
//  author: David Fridrich                                                    //
//  file: secret.c - implementation of functions to transfer file through icmp//
//  last updated: 02.11.2021                                                  //
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
 * this is defined because of addrfilelen_char struct in netdb.h (check the definition)
 * it is defined under a condition a XOPEN2K POSIX is defined, which is fairly
 * common in g++ or using -std=gnuXX but not in gcc -std=cXX, therefore
*/
/**
 * #2:
 * defining XOPEN_SOURCE, compatibility with pcap.h doesn't work because
 * of the unsigned chars/ints defined in sys/types.h (there is a #define
 * limitation for __USE_MISC which isn't met, therefore I'm using standard gcc)
*/
// #define _XOPEN_SOURCE 700

//define some constants (packet max size, max file len in bytes, max file len)
#define PACKET_MAX_SIZE 1500
#define FILE_LEN_BYTES 5 //(0 to 1,099,511,627,775 ~ 1TB)
#define MAX_FILE_LEN 1099511627775

// more on this: https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
// verbose print
#define verbose_print(fmt, ...) \
	do { if (ptr->_verbose_printing) printf("verbose: "fmt, __VA_ARGS__); } while(0)


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

#include "secret.h"

enum who_am_i {
	IS_SERVER,
	IS_CLIENT,
};

char *encryptionKey = "xfridr08";


// ########################################################################## //
//																functions																		//


void init_settings(){
	
	ptr = malloc(sizeof(settings));
	if(!ptr){
		printErr("Malloc failed @init_settings()");
	}
	ptr->verbose_printing = 0; //default
	ptr->is_server 				= 0;
	ptr->file_name 				= NULL;
}

void printErr(char *s){
	fprintf(stderr,"Error: %s\n",s);
	exit(1);
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

void fileExists(char *file){
	if( access(file,F_OK | R_OK) != 0) {
		printErr("File doesn't exist and/or has wrong read permissions set");
	}
}

char *getFilename(char *path){

// find backslashes
	int backslashPos = -1;
	for (int i = strlen(path)-1; i >= 0; --i){
		if (path[i] == '/'){
			backslashPos = i;
			break;
		}
	}
	if(backslashPos < 0){
		return (path);
	}
	
	char *f = malloc(strlen(path)-backslashPos);
	memcpy(f,path+backslashPos+1,strlen(path)-backslashPos);
	f[strlen(f)] == '\0';

	return f;
}

char *fileOpenReadBytes(char *in, long unsigned int *len, char **filename){
	char* ret;

	*filename = getFilename(in);
	printf("here, a file: %s\n",*filename); //DEBUG PRINT
	// code snippet to open file in binary mode
	//https://stackoverflow.com/a/22059317
	FILE *f = fopen(in,"rb"); //open in binary mode
	fseek(f,0,SEEK_END);
	// *len = ftell(f);
	long long unsigned int tmp = ftell(f);
	if(sizeof(*len) == 8){ //64bit OS
		if(tmp > MAX_FILE_LEN){
			free(in);
			fclose(f);
			printErr("Size of file exceeds MAX_FILE_LEN (~1 Tb) - what are you trying to send, NASA TOP SECRET DOCUMENTS?");
		}
	} else { //32bit OS
		if (tmp > UINT32_MAX){
			free(in);
			fclose(f);
			printErr("Size of file exceeds UINT32_MAX (~4 Gbs)");
		}
	}
	rewind(f);
	
	*len = (unsigned long int)tmp;

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
	if(argc == 1){
		printHelp();
	}

	/*
   * parameter -l is exclusive.
	 *  if -l option is given, no other parameters can be given(except verbose).
	*/
  for (int i = 1; i < argc; i++){
    if(!strcmp(argv[i],"-h")||!strcmp(argv[i],"--help")){
      printHelp();
    }
    else if(!strcmp(argv[i],"-l")){ //is server
      ptr->is_server = 1;
    }
    else if(!strcmp(argv[i],"-s")){ //host
      if(argv[++i] != NULL){
        *host = malloc(strlen(argv[i])+1);
				memcpy(*host,argv[i],strlen(argv[i])+1);
      } else {
        printErr("Parameter for -s (host) was not provided");
      }
    }
    else if(!strcmp(argv[i],"-r")){ //file
      if(argv[++i] != NULL){
				fileExists(argv[i]);
        *file = malloc(strlen(argv[i])+1);
        memcpy(*file,argv[i],strlen(argv[i])+1);
				
      } else {
        printErr("Parameter for -r (file) was not provided");
      }
    }
		else if(!strcmp(argv[i],"-v")){//verbose printing
			ptr->verbose_printing = 1;
		} 
		
		else {
      printErr("Unknown parameter was given");
    }
  }

  // check parameters

  if(ptr->is_server && (*file != NULL || *host != NULL)){
    printErr("Parameter -l is exclusive. It can't be mixed with parameters -r & -s");
  }

	return ptr->is_server ? IS_SERVER : IS_CLIENT;
}

// ------------------ CHECKSUM FUNC ------------------ //

// this function is taken from:
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
	 * then resolve IP header (IPv4 == 20 bytes) --> any useful filelen_char in here?
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
		printErr("Not all necessary parameters were given.");
	}

	/**
	 * -- unsigned long int sizes --
	 * 32bit-OS:
	 * 	max value is ~4kkk bytes (4,294,967,296b - UINT32_MAX)
	 * 
	 * 64bit-OS:
	 * max value is ~18kkkkkk bytes (18,446,744,073,709,551,615b - UINT64_MAX)
	*/
	long unsigned int filelen;
	char *filename;
	char *filebuff = fileOpenReadBytes(file,&filelen,&filename);

	if (filelen == 0){
		free(filename); // TODO resolve this bad boy
		free(filebuff);
		free(host);
		free(file);
		printErr("Given file is empty, stopping");
	}

	// free(filename); //DEBUG
	// free(filebuff); //DEBUG
	// exit(0); //DEBUG

	int fd; //file descriptor for socket
	struct addrinfo *target,*tmp,hints;

	memset(&hints,0,sizeof(hints));

	hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
	hints.ai_socktype = SOCK_RAW;

// gethostbyname is obsolete, use -> getaddrfilelen_char()
	int tmpRet = getaddrinfo(host,NULL,&hints,&tmp);
	if(tmpRet != 0){
		fprintf(stderr, "getaddrfilelen_char: %s\n", gai_strerror(tmpRet));
		exit(1);
	}

	// snippet taken from https://man7.org/linux/man-pages/man3/getaddrfilelen_char.3.html
	// getaddrfilelen_char returns in 'pai' (last argument) a list of address structures
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
		//socket is successfully created
		break;
	}


	struct sockaddr servinfo = *(struct sockaddr*)target->ai_addr;
	socklen_t servlen = target->ai_addrlen;
	
	freeaddrinfo(tmp);

	if(target == NULL){
		printErr("No socket was created successfully, maybe you forgot sudo?\n");
	}

	/**
	 * PACKET
	 * [0-7] -- icmp header
	 * [8-9]32bit / [8-11]64bit -- sequence number
	 * 
	 * first packet: 
	 * 			[12-16] -- total length of file to be transfered (char filelen_char[])
	 * 			+ data
	 * 
	 * not first packet:
	 * 			+ data
	 * 			
	*/

	//how many bytes are used in packet already
	int used = 0;

	//init packet -- largest possible packet size is set to 1500B
	char packet[PACKET_MAX_SIZE];
	memset(&packet,0,PACKET_MAX_SIZE);

	// ICMP header length is 8 bytes
	struct icmphdr *icmpH = (struct icmphdr*)packet;
	int icmpHlen = sizeof(struct icmphdr);
	icmpH->code = ICMP_ECHO;
	icmpH->checksum = 0;

	// max value is a number that fits in 5 bytes
	char filelen_char[FILE_LEN_BYTES];
	sprintf(filelen_char,"%d",filelen); //done DEBUG

	printf("filelen num (%d)\n",filelen); //DEBUG
	printf("strlen(filelen_char) alloc'd (%d)\n",strlen(filelen_char)); //DEBUG

	//copy icmp header to the start of the packet -- this is always the same
	memcpy(packet,icmpH,icmpHlen);
	used += icmpHlen;

	unsigned int seqNum = 1;
	char seqNum_char[sizeof(unsigned int)] = {0};
	sprintf(seqNum_char,"%d",seqNum);

	// copy sequence number to packet
	memcpy(packet+used,seqNum_char,sizeof(seqNum_char));
	used += sizeof(seqNum_char);

	// ***** for the first packet only ***** //
	// copy file name
	memcpy(packet+used,filename,strlen(filename));
	used += strlen(filename);

	// copy file length info to packet
	memcpy(packet+used,filelen_char,FILE_LEN_BYTES);
	used += FILE_LEN_BYTES;
	// ***** for the first packet only ***** //

	unsigned int totalBytesSent = 0;
	unsigned int datasize = 0;

	char* moving_file_ptr = filebuff;

	//send packets until whole file is sent
	do {

		datasize = filelen-totalBytesSent < PACKET_MAX_SIZE-used ? filelen-totalBytesSent : PACKET_MAX_SIZE-used;
		
		// copy file data
		memcpy(packet+used,moving_file_ptr,datasize);

		//calculate checksum (IPv4 only)
		icmpH->checksum = checksum(packet,used+datasize,protocol);
		
		if (sendto(fd,packet,used+datasize,0,&servinfo,servlen) < 0){
			//cleaning process
			printErr("Function sendto failed, exiting.\n");
		}
		
		totalBytesSent+=datasize;
		moving_file_ptr = &filebuff[totalBytesSent];
		seqNum++;

		sprintf(seqNum_char,"%d",seqNum);
		used = icmpHlen+sizeof(seqNum_char); //standard size of headers

		// copy sequence number to packet
		memcpy(packet+icmpHlen,seqNum_char,sizeof(seqNum_char));

	} while(totalBytesSent<filelen);



	// // if there is more data to be sent, continue sending
	// while(fileBytesSent < filelen){

	// 	used = icmpHlen+seqNum;

	// 	//last packet to send
	// 	if (filelen-fileBytesSent < PACKET_MAX_SIZE-used){
	// 		available = filelen-fileBytesSent;
	// 	}

	// 	memcpy(packet+used,filebuff,available);

	// 	*filebuff+=available;
	// 	fileBytesSent += available;

	// 	//calculate checksum
	// if(protocol==IPPROTO_ICMP){
	// 	icmpH->checksum = checksum(packet,used+available);
	// }

	// 	printf("%d: Sent %d bytes\n",seq_num++,available+used);
	// 	// https://man7.org/linux/man-pages/man3/sendto.3p.html
	// 	if (sendto(fd,packet,used+available,0,&servinfo,servlen) < 0){
	// 		printErr("Function sendto failed, exiting.\n");
	// 	}
	// 	used = icmpHlen;
	// 	available = PACKET_MAX_SIZE-used;
	// }
	printf("done!\n");
	//encryption


}




// --------------------------------- MAIN --------------------------------- //
int main(int argc, char **argv){

	//initialize some values
	char *file = NULL; // r option
	char *host = NULL; // s option

	init_settings();
	
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