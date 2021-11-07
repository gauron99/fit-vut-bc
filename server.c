

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



char *getFilenameFromPacket(const u_char *p){

}

unsigned int getSeqNumFromPacket(const u_char *p){

}

unsigned int getFileLenFromPacket(const u_char *p){

}

int saveDataToFile(const u_char* data, unsigned int sn){
	static unsigned int fileLen = 0;
	static unsigned int seqNum = 0;
	
	if(sn == 1){
		ptr->file_name = getFilenameFromPacket(data);
		seqNum = getSeqNumFromPacket(data);
		fileLen = getFileLenFromPacket(data);

		//file exists & has read permission & is empty & has write permission
		if(fileExists(ptr->file_name) && fileIsEmpty(ptr->file_name) && (!access(ptr->file_name,W_OK))){
			createFile();

		} else {
			//create new file (one backup)
			int fLen = strlen(ptr->file_name);
			char *tmpt = malloc(fLen+5);
			memcpy(tmpt,"isa_",4);
			memcpy(tmpt+4,ptr->file_name,strlen(ptr->file_name)+1);
			memcpy(ptr->file_name,tmpt,strlen(tmpt)+1);
			free(tmpt);

			createFile();
		}
	}
	//skip icmp header
	data += sizeof(struct icmphdr);
	
	//decryption


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
				saveDataToFile(packet+iphLen,++seq_num);
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