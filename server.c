

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

char *getFilenameFromPacket(char *p, unsigned int *r){
// 	0-2b 				-- filename len = X
// 	3-Xb 				-- filename

	// get length of filename
	char fnl_buff[MAX_FILE_NAME_LEN];
	memcpy(fnl_buff,p,MAX_FILE_NAME_LEN);
	*r += MAX_FILE_NAME_LEN;

	// printf("r:%d|fnl:%s\n",*r,fnl_buff);

	char *endptr = NULL;
	int fnl = (int)strtol(fnl_buff,&endptr,10);
	if(*endptr != '\0'){
		printErr("strtol() failed @getFilenameFromPacket()");
	}
	
	// get name of file
	char *fname = calloc(fnl,1); // +1 for '\0' included in fnl
	if(fname == NULL){
		printErr("Calloc() failed @getFilenameFromPacket()");
	}

	memcpy(fname,p+MAX_FILE_NAME_LEN,fnl);
	*r += fnl;
	
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

unsigned char* decryptData(const unsigned char* d){
	extern const char* encryptionKey;
	
	int exclude = sizeof(struct icmphdr);

	AES_KEY dec_key;
	AES_set_decrypt_key((unsigned char*)encryptionKey,128,&dec_key);

	unsigned char *ret = calloc(PACKET_MAX_SIZE-exclude,1);

	// decrypt everything except icmp header
	int i;
	for (i = 0; i < (PACKET_MAX_SIZE-exclude); i+=AES_BLOCK_SIZE){
		AES_decrypt(d+i,ret+i,&dec_key);
	}

	// printf("INBOI[i:%d;calloced:%d]\n\n",i,PACKET_MAX_SIZE-exclude);
	// for (size_t i = 0; i < (PACKET_MAX_SIZE-exclude); i++){
	// 	printf("%c",ret[i]);
	// }
	// printf("\n");
	return ret;
}

void prepOutputFile(){
	//file doesnt exist OR (file exists & has read permission && is empty && has write permission)
		if(!fileExists(ptr->file_name) || (fileExists(ptr->file_name) && fileIsEmpty(ptr->file_name) && (!access(ptr->file_name,W_OK)))){
			//do nothing
		} else {
			//create new file (one backup)
			int fLen = strlen(ptr->file_name);

			char* tmp1 = malloc(fLen);
			memcpy(tmp1,ptr->file_name,fLen);

			ptr->file_name = realloc(ptr->file_name,fLen+5);
			memset(ptr->file_name,0,fLen);

			memcpy(ptr->file_name,"isa_",4);
			memcpy(ptr->file_name+4,tmp1,fLen);
			free(tmp1);
		}

	// open file for writing
	ptr->f = fopen(ptr->file_name,"ab");
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
	
	//packet validation
	if((ntohs(icmpH->un.echo.id) != PACKET_ID) || (sn != sn_packet) ){
		return -1; //wrong packet, do nothing
	}
	// printf("--- ICMP struktura ---\n");//DEBUG
	// printf("checksum:%d, id:%d, sn: %d\n------------\n",icmpH->checksum,ntohs(icmpH->un.echo.id),sn_packet);//DEBUG

	//says how many bytes have been read from packet so far(useful for skiping 
	//bytes before file data)
	unsigned int read = 0;

	//first packet
	if(sn == 1){
		
		// int k,l;
		// for (k = 0; k < 1472; k+=AES_BLOCK_SIZE)
		// {
		// 	for (l = 0; l < AES_BLOCK_SIZE; l++)
		// 	{
		// 		printf("%X ",data[8+k+l]);
		// 	}
		// 	printf("\n--\n");
		// }
		// printf("i:%d\n",k);

		char *decr_data = (char*)decryptData(data+icmpHlen); //save file data to ptr->filebuff
		
		ptr->file_name = getFilenameFromPacket(decr_data,&read);

		prepOutputFile();

		fileLen = getFileLenFromPacket(decr_data,&read);
		printf("fileLen: %d\n",fileLen);//DEBUG
		printf("transfered:%d,read:%d,fl:%d\n",transfered,read,fileLen);

		dip = getMaxDataAvailable(read+icmpHlen,transfered,fileLen);
		// printf("dip:%d\n",dip);

		//init filebuff
		ptr->filebuff = calloc(dip,1);
		
		// printf("------------\n");
		// for (int i = 0; i < dip; i++){
		// 	printf("%c,",decr_data[read+i]);
		// }
		// printf("\n");
		// exit(0);

		memcpy(ptr->filebuff,decr_data+read,dip);
		transfered += dip;
		
	} else { //NOT first packet (every other one)
		read = icmpHlen;

		unsigned char* decr_data = decryptData(data+icmpHlen);
		
		dip = getMaxDataAvailable(read,transfered,fileLen);

		// printf("decrypted DATA:\n");
		// for (size_t i = 0; i < dip; i++)
		// {
		// 	printf("%c,",decr_data[read+i]);
		// }
		// printf("\n");

		memcpy(ptr->filebuff,decr_data,dip);

		transfered += dip;

	}
	
	// append to file
	if (fwrite(ptr->filebuff,dip,1,ptr->f) != 1){
		printErr("Fwrite returned unexpected value");
	}

	//all data has been transfered
	if(transfered == fileLen){

		// printf(">>%s<<\n",ptr->filebuff);

		// printf("~~ File transfer done! ~~\n");
		printf("DONE! Wrote to file: %s\n",ptr->file_name);
		transfered = 0;
		fileLen = 0;

		fclose(ptr->f);
		free(ptr->filebuff);
		free(ptr->file_name);
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

	static unsigned int seq_num = 0;

		//ip header
		iph = (struct ip*) (packet + SIZE_LCC);
    iphLen = iph->ip_hl*4; // == 20

		//icmp header
		struct icmphdr *icmph = (struct icmphdr*)(packet + SIZE_LCC + iphLen);

    switch (iph->ip_p){
		case 1:
			if(icmph->type == ICMP_ECHO){
				if(handleData((unsigned char*)(packet + SIZE_LCC + iphLen),++seq_num)){
					//if one is returned, whole file transfer done!
					seq_num = 0;
				}
				//send ack packet back ?
			}
			break;
		case 58: // IPv6-ICMP
			printf("%d: another one! - IPv6 \n",++seq_num);
		default:
			// printf("protocol unknown: %d\n",iph->ip_p);
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