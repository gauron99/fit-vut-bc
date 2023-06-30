

#include <pcap.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h> 
#include <arpa/inet.h>
#include <netinet/ip.h> 			// struct iphdr (+server)

#include "secret.h"

extern struct settings *ptr;
pcap_t *handler;         				// packet capture handle 

/**
 * When capturing from the "any" device, or from one of those other devices,
 * in Linux, the libpcap doesn't supply the link-layer header for the real 
 * "hardware protocol" like Ethernet, but instead supplies a fake link-layer 
 * header for this pseudo-protocol. (like "linux cooked capture")
 * https://wiki.wireshark.org/SLL
*/ 


void getFilenameFromPacket(char *p, unsigned int *r){
// 	0-2b 				-- filename len = X
// 	3-Xb 				-- filename

	// get length of filename
	char fnl_buff[MAX_FILE_NAME_LEN];
	memcpy(fnl_buff,p,MAX_FILE_NAME_LEN);
	*r += MAX_FILE_NAME_LEN;

	char *endptr = NULL;
	int fnl = (int)strtol(fnl_buff,&endptr,10);
	if(*endptr != '\0'){
		printErr("strtol() failed @getFilenameFromPacket()");
	}
	// get name of file
	ptr->file_name = calloc(fnl+1,1);//+1 for '\0'
	if(ptr->file_name == NULL){
		printErr("Calloc() failed @getFilenameFromPacket()");
	}

	memcpy(ptr->file_name,p+MAX_FILE_NAME_LEN,fnl);
	ptr->file_name[fnl]='\0'; //add terminating null char
	*r += fnl;
}

unsigned int getFileLenFromPacket(char *p, unsigned int *r){
// 	(X-(X+13))b	-- file data len = Y

	char tmp[FILE_LEN_BYTES];
	memcpy(tmp,p+*r,FILE_LEN_BYTES);
	*r += FILE_LEN_BYTES;

	char *endptr = NULL;
	unsigned int fl = strtoul(tmp,&endptr,10);
	if(fl == 0 || *endptr != '\0' || endptr == tmp){
		printErr("strtoul() failed @getFileLenFromPacket()");
	}
	return fl;
}

unsigned char* decryptData(const unsigned char* d){
	extern const char* encryptionKey;
	
	int exclude = sizeof(struct icmphdr);

	AES_KEY key;
	AES_set_decrypt_key((unsigned char*)encryptionKey,128,&key);
	unsigned char *ret = calloc(DATA_MAX_SIZE-exclude,1);
	// decrypt everything except icmp header
	int i;
	for (i= 0; i < (DATA_MAX_SIZE-exclude); i+=AES_BLOCK_SIZE){
		AES_decrypt(d+i,ret+i,&key);
	}
	return ret;
}

void prepOutputFile(){
	//file doesnt exist OR (file exists & has read permission && is empty && has write permission)
		if(!fileExists(ptr->file_name) || (fileExists(ptr->file_name) && fileIsEmpty(ptr->file_name) && (!access(ptr->file_name,W_OK)))){
			//do nothing
		} else {
			//create new file (one backup)
			int fLen = strlen(ptr->file_name);
			char* tmp = malloc(fLen+1);
			if(tmp==NULL){
				printErr("Malloc failed @prepOutputFile()");
			}
			memcpy(tmp,ptr->file_name,fLen);

			char* tmp_reall = realloc(ptr->file_name,fLen+5);
			if(tmp_reall == NULL){
				printErr("Realloc failed @(prepOutputFile()");
			}
			ptr->file_name = tmp_reall;
			
			memset(ptr->file_name,0,fLen);

			memcpy(ptr->file_name,"isa_",4);
			memcpy(ptr->file_name+4,tmp,fLen);
			ptr->file_name[fLen+4] = '\0';
			free(tmp);
		}

	// open file for writing
	ptr->f = fopen(ptr->file_name,"wb");
	if(ptr->f == NULL){
		printErr("File couldn't be opened(failed w/ 'ab' mode)[server-side]");
	}
}

int handleData(unsigned char* data, unsigned int sn){
	static unsigned int fileLen = 0;
	static unsigned int transfered = 0;
	int dip = 0;

	struct icmphdr *icmpH = (struct icmphdr*)data;
	int icmpHlen = sizeof(struct icmphdr);

	unsigned int sn_packet = ntohs(icmpH->un.echo.sequence);
	
	//packet validation (ID & sequence number)
	if((ntohs(icmpH->un.echo.id) != PACKET_ID) || (sn != sn_packet) ){
		return -1; //wrong packet, do nothing
	}
	printf("%d caught!\n",sn_packet);

	//says how many bytes have been read from packet so far(useful for skiping 
	//bytes before file data)
	unsigned int read = 0;

	//first packet
	if(sn == 1){

		char *decr_data = (char*)decryptData(data+icmpHlen); //save file data to ptr->filebuff
		
		getFilenameFromPacket(decr_data,&read);

		prepOutputFile();	

		fileLen = getFileLenFromPacket(decr_data,&read);

		dip = getMaxDataAvailable(read+icmpHlen,transfered,fileLen);

		//init filebuff
		ptr->filebuff = calloc(dip,1);
		memcpy(ptr->filebuff,decr_data+read,dip);
		free(decr_data);
		transfered += dip;
		
	} else { //NOT first packet (every other one)
		read = icmpHlen;

		unsigned char* decr_data = decryptData(data+icmpHlen);
		
		dip = getMaxDataAvailable(read,transfered,fileLen);

		ptr->filebuff = calloc(dip,1);
		memcpy(ptr->filebuff,decr_data,dip);
		free(decr_data);

		transfered += dip;
	}
	
	// append to file
	if (fwrite(ptr->filebuff,dip,1,ptr->f) != 1){
		printErr("Fwrite returned unexpected value");
	} else {
		free(ptr->filebuff);
		ptr->filebuff = NULL;
	}

	//all data has been transfered
	if(transfered == fileLen){

		printf("DONE! Wrote to file: %s\n",ptr->file_name);
		transfered = 0;
		fileLen = 0;

		fclose(ptr->f);
		ptr->f = NULL;

		free(ptr->file_name);
		ptr->file_name = NULL;

		return 1;
	}
	return 0;
}

/**
 * handle captured packets here
 * this is a pcap_loop() callback function
 * 
 * sources:
 * https://www.binarytides.com/packet-sniffer-code-c-libpcap-linux-sockets/
 * https://www.binarytides.com/packet-sniffer-code-c-linux/
*/
void packet_hdlr_cb(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){

  (void)args;
  (void)header;

	struct ip *iph;
  u_int iphLen;
	u_int iphV6Len = 40; //size of ipv6 standard header is 40b

	static unsigned int seq_num = 0;

		//ip header
		iph = (struct ip*) (packet + SIZE_LCC);
    iphLen = iph->ip_hl*4; // standard of 20b

		//icmp header
		struct icmphdr *icmph = (struct icmphdr*)(packet + SIZE_LCC + iphLen);

    switch (iph->ip_p){
		case 1:
			if(icmph->type == ICMP_ECHO){
					if((handleData((unsigned char*)(packet + SIZE_LCC + iphLen),++seq_num))==1){
						seq_num = 0;
					}
			}
			break;
		case 58: // IPv6-ICMP
			if(icmph->type == ICMP_ECHO){
					if((handleData((unsigned char*)(packet + SIZE_LCC + iphV6Len),++seq_num))==1){
						seq_num = 0;
					}
			}
		default:
			break;
			}
}

// ------------------ SERVER FUNC ------------------ //

/*
 * inspiration from:
 * 'sniff-filter.c' example file in "isa/soubory" from 'Petr Matousek, 2020'
*/
void server() {
	char errbuf[PCAP_ERRBUF_SIZE];  // constant defined in pcap.h
  pcap_if_t *alldev, *dev ;       // all available devices

  char *devname = NULL;           // a name of the chosen device

  bpf_u_int32 netaddr;            // network address configured at the input device
  bpf_u_int32 mask;               // network mask of the input device
  
	struct bpf_program filter;      // the compiled filter

	// open the input devices (interfaces) to choose from
  if (pcap_findalldevs(&alldev, errbuf)){

    // printErr("Can't open input device(s)");
		fprintf(stderr,"Error: pcap_findalldevs(): %s\n",errbuf);
		exit(1);
	}

  for (dev = alldev; dev != NULL; dev = dev->next){
		if(!strcmp(dev->name,"any")){
		  devname = dev->name;
			break;
		}
  }
	if(devname == NULL){
		devname = alldev->name;
	}
	// devname = alldev->name;

	printf("devname:%s\n",devname);

	// get IP address and mask of the sniffing interface
  if (pcap_lookupnet(devname,&netaddr,&mask,errbuf) == -1){
    // printErr("Can't get IP & mask of chosen interface");
		fprintf(stderr,"Error: pcap_lookupnet(): %s\n",errbuf);
		exit(1);
	}

	//open the interface
	if((handler = pcap_open_live(devname,PACKET_MAX_SIZE,1,1000,errbuf)) == NULL){
		fprintf(stderr,"Error: pcap_open_live(): %s\n",errbuf);
		exit(1);
	}

	//add custom filter & apply
	if(pcap_compile(handler,&filter,"icmp or icmp6",0,netaddr) == -1){
		fprintf(stderr,"Error: pcap_compile(): %s\n",errbuf);
		exit(1);
	}


	if(pcap_setfilter(handler,&filter) == -1){
		fprintf(stderr,"Error: pcap_setfilter(): %s\n",errbuf);
		exit(1);
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