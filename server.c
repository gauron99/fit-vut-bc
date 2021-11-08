

#include <pcap.h>
#include <pcap/pcap.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h> 
#include <arpa/inet.h>
#include <netinet/ip.h> 			// struct iphdr (+server)

#include "secret.h"

extern struct settings *ptr;

/**
 * When capturing from the "any" device, or from one of those other devices,
 * in Linux, the libpcap doesn't supply the link-layer header for the real 
 * "hardware protocol" like Ethernet, but instead supplies a fake link-layer 
 * header for this pseudo-protocol. (like "linux cooked capture")
 * https://wiki.wireshark.org/SLL
*/ 


/** 0-3b 				-- filename len = X
 * 	4-Xb 				-- filename
 * 	(X-(X+13))b	-- file data len = Y
 * 	((X+13)-Y)b	-- data
 * 	-- if data is smaller than PACKET_MAX_LEN (dont read)
*/



char *getFilenameFromPacket(char *p, unsigned int *r){
// 	0-2b 				-- filename len = X
// 	3-Xb 				-- filename

	// get length of filename
	char fnl_buff[MAX_FILE_NAME_LEN];
	memcpy(fnl_buff,p,MAX_FILE_NAME_LEN);
	*r += MAX_FILE_NAME_LEN;

	char *endptr = NULL;
	int fnl = (int)strtol(fnl_buff,&endptr,10);
	if(*endptr != '\0' || fnl_buff == endptr){
		printErr("strtol() failed @getFilenameFromPacket()");
	}
	
	// get name of file
	char *fname = calloc(fnl,1); // +1 for '\0' included in fnl
	if(fname == NULL){
		printErr("Calloc() failed @getFilenameFromPacket()");
	}

	memcpy(fname,p+(*r),fnl);
	(*r) += fnl;
	
	return fname;
}

unsigned int getFileLenFromPacket(char *p, unsigned int *r){
// 	(X-(X+13))b	-- file data len = Y
	char tmp[FILE_LEN_BYTES];
	memcpy(tmp,p+*r,FILE_LEN_BYTES);
	*r += FILE_LEN_BYTES;

	char *endptr = NULL;
	unsigned int fl = strtoul(tmp,&endptr,10);
	if(fl == 0 || *endptr != '\0' || endptr == tmp){
		printf("fl: %d",fl);
		printErr("strtoul() failed @getFileLenFromPacket()");
	}
	return fl;
}

// void handleFirstPacket(const u_char* data,struct icmphdr *icmpH,unsigned int sn){
// 	int used = sizeof(icmpH);
// 	// decrypt data
// 	// decryptPacketData(data);
// }

unsigned char* decryptData(const unsigned char* d){
	extern const unsigned char* encryptionKey;

	AES_KEY dec_key;
	AES_set_decrypt_key(encryptionKey,128,&dec_key);
	unsigned char *ret = calloc(PACKET_MAX_SIZE-8,1);

	// decrypt everything except icmp header
	for (int i = 0; i < PACKET_MAX_SIZE-sizeof(struct icmphdr); i+=AES_BLOCK_SIZE){
		// printf("i:%d,",i);//DEBUG
		AES_decrypt(d+i,ret+i,&dec_key);
		
	}
	// printf("\n----------\n");//DEBUG
	return ret;
}

int handleData(const u_char* data, unsigned int sn){
	static unsigned int fileLen = 0;
	static unsigned int transfered = 0;

	struct icmphdr *icmpH = (struct icmphdr*)data;
	unsigned int sn_packet = ntohs(icmpH->un.echo.sequence);
	
	//packet validation
	if((ntohs(icmpH->un.echo.id) != PACKET_ID) || (sn != sn_packet) ){
		return -1; //wrong packet, do nothing
	}

	printf("--- ICMP struktura ---\n");//DEBUG
	printf("checksum:%d, id:%d, sn: %d\n",icmpH->checksum,ntohs(icmpH->un.echo.id),sn_packet);//DEBUG

	//says how many bytes have been read from packet so far(useful for skiping 
	//bytes before file data)
	unsigned int read = 0;

	//first packet
	if(sn == 1){
		printf("AM FIRST BOI\n");
		char* decr_data = (char*)decryptData(data+sizeof(icmpH)); //save file data to ptr->filebuff
		// printf("------------\n");
		// for (int i = 0; i < 33; i++)
		// {
		// 	printf("%c,",decr_data[i]);
		// }
		// printf("\n");
		// exit(0);//DEBUG

		ptr->file_name = getFilenameFromPacket(decr_data,&read);
		printf("filename: %s\n",ptr->file_name);//DEBUG

		fileLen = getFileLenFromPacket(decr_data,&read);
		printf("fileLen: %d\n",fileLen);//DEBUG

		//init filebuff
		ptr->filebuff = calloc(fileLen,1);

		// printf("decrypted DATA:\n");
		// for (size_t i = 0; i < fileLen; i++)
		// {
		// 	printf("%c,",decr_data[read+i]);
		// }
		// printf("\n");

		//add icmp header to "read" bytes (before file data)
		read += sizeof(icmpH);

		// copy data to buffer
		int dip = getMaxDataAvailable(read,transfered,fileLen);
		memcpy(ptr->filebuff+transfered,decr_data+read,dip);
		transfered += dip;

		//file doesnt exist OR (file exists & has read permission && is empty && has write permission)
		if(!fileExists(ptr->file_name) || (fileExists(ptr->file_name) && fileIsEmpty(ptr->file_name) && (!access(ptr->file_name,W_OK)))){
			//do nothing
		} else {
			//create new file (one backup)
			int fLen = strlen(ptr->file_name); //TODO realloc?
			char *tmpt = malloc(fLen+5);
			memcpy(tmpt,"isa_",4);
			memcpy(tmpt+4,ptr->file_name,strlen(ptr->file_name)+1);
			memcpy(ptr->file_name,tmpt,strlen(tmpt)+1);
			free(tmpt);
		}

		//NOT first packet (every other one)
	} else {

	}
	
	//all data has been transfered
	if(transfered == fileLen){

		printf(">%s<\n",ptr->filebuff);


		FILE *f = fopen(ptr->file_name,"wb+");
		if(f == NULL){
			printErr("File couldn't be opened(failed 'wb' mode)[server-side]");
		}
		fwrite(ptr->filebuff,fileLen,1,f);
		fclose(f);
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

	static unsigned int seq_num = 0;

		// skip ethernet layer
		packet += SIZE_LCC;

		//ip header
		iph = (struct ip*) packet;
    iphLen = iph->ip_hl*4;

		//icmp header
		struct icmphdr *icmph = (struct icmphdr*)(packet + iphLen);

    switch (iph->ip_p){
		case 1:
			if(icmph->type == ICMP_ECHO){
				// printf("%d: another one! - IPv4[code:%d]\n",++seq_num,icmph->type);
				handleData(packet+iphLen,++seq_num);
			}
			break;
		case 58: // IPv6-ICMP
			printf("%d: another one! - IPv6 \n",++seq_num);
		default:
			printf("protocol unknown: %d\n",iph->ip_p);
			return;
  
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
	if(devname == NULL){
		devname = alldev->name;
	}
	// devname = alldev->name;

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