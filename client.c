
#include "secret.h"


extern struct settings *ptr;


int getMaxDataAvailable(int used,int sent,int fl){
  return (fl-sent) < (PACKET_MAX_SIZE-used) ? (fl-sent) : (PACKET_MAX_SIZE-used);
}

int createFirstPacket(char (*p)[PACKET_MAX_SIZE], int used, unsigned int l){
  
  unsigned int fnsi = strlen(ptr->file_name);
  if(fnsi>999){
    printErr("Length of file name is larger than allowed (999)");
  } else {
    char fns[MAX_FILE_NAME_LEN] = {0};
    if(sprintf(fns,"%u",fnsi) < 0){
      printErr("Sprintf() failed @createFirstPacket");
    }
    memcpy(*p + used,fns,MAX_FILE_NAME_LEN);
    used += MAX_FILE_NAME_LEN;
  }

  // copy file name
	memcpy(*p + used,ptr->file_name,fnsi);
	used += fnsi;

	// max value is a number that fits in 13 bytes
	char filelen_char[FILE_LEN_BYTES] = {0};
	sprintf(filelen_char,"%u",l);

	// copy file length info to packet
	memcpy(*p + used,filelen_char,FILE_LEN_BYTES);
	used += FILE_LEN_BYTES;

  printf("file_name: %s\n" ,ptr->file_name);//DEBUG
	printf("filelen num (%u)\n",l); //DEBUG

  return used;
}

// ------------------ CLIENT FUNC ------------------ //

void client(char *file, char *host){
	if( (!strcmp(file,"")) || (!strcmp(host,"")) || file == NULL || host == NULL ){
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
	ptr->filebuff = fileOpenReadBytes(file,&filelen);

	ptr->file_name = getFilename(file);
	printf("here, a file: %s\n",ptr->file_name); //DEBUG PRINT
	
	if (filelen == 0){
		free(ptr->file_name); // TODO resolve this bad boy
		free(host);
		free(file);
		printErr("Given file is empty");
	}

	// free(filename); //DEBUG
	// free(filebuff); //DEBUG
	// exit(0); //DEBUG

	int fd; //file descriptor for socket
	struct addrinfo *target,*tmp,hints;

	memset(&hints,0,sizeof(hints));

	//if localhost -> IPv4
	if(!(strcmp("localhost",host))){
		hints.ai_family = AF_INET;
	} else {
		hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
	}
	
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
		printf("addr:%s\n",target->ai_addr->sa_family==2?"ipv4":"ipv6 or other");//DEBUG
		printf("family:%s\n",target->ai_family==2?"ipv4":"icmp6 or other");//DEBUG
		if(protocol != 1){
			printf("protocol:%s(val: %d)\n", "ICMP6 maybe?",protocol);//DEBUG
		}
		
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
	 * PACKET -- is of size 1480 (1500-20 for IP header -is asigned automatically thanks to SOCK_RAW or some)
   * 
   * first packet:
	 * [0-7]      -- icmp header
	 * [8-10]     -- file_name length = X
   * [11-(11+X)]-- file_name
	 * [X-(X+13)] -- file length (MAX_FILE_LEN in secret.h)
   * 
	*/

	//how many bytes are used in packet already
	int used = 0;

	//init packet -- largest possible packet size is set to 1480B (MTU - IPheader(20bytes))
	char packet[PACKET_MAX_SIZE];
	memset(&packet,0,PACKET_MAX_SIZE);

	// ICMP header setup default values
	struct icmphdr *icmpH = (struct icmphdr*)packet;
	int icmpHlen = sizeof(struct icmphdr);
	icmpH->type = ICMP_ECHO;
  icmpH->un.frag.mtu=1500;
  icmpH->un.echo.sequence = 0x0100;   //TODO

  icmpH->un.echo.id = 0x6666; //default  

  used += icmpHlen;

	// unsigned int seqNum = 1;
	// char seqNum_char[sizeof(unsigned int)] = {0};
	// sprintf(seqNum_char,"%d",seqNum);

	// copy sequence number to packet
	// memcpy(packet+used,seqNum_char,sizeof(seqNum_char));
	// used += sizeof(seqNum_char);

	unsigned int totalBytesSent = 0;
	unsigned int datasize = 0;

	char* moving_file_ptr = ptr->filebuff;

  used += createFirstPacket(&packet,used,filelen);

  datasize = getMaxDataAvailable(used,totalBytesSent,filelen);


  memcpy(packet+used,ptr->filebuff,datasize);
  // encryptData()

  icmpH->checksum = checksum(packet,used+datasize,protocol);

  //send first packet with additional information
  if(sendto(fd,packet,used+datasize,0,&servinfo,servlen) < 0){
    printErr("sendto() failed for the first packet");
  } else {  
    icmpH->un.echo.sequence += 0x0100;
  }

  totalBytesSent = datasize;

	//send packets until whole file is sent
	while(totalBytesSent<filelen) {

		datasize = filelen-totalBytesSent < PACKET_MAX_SIZE-used ? filelen-totalBytesSent : PACKET_MAX_SIZE-used;
		
		//encryption TODO
		
		// copy file data
		memcpy(packet+used,moving_file_ptr,datasize);

		//calculate checksum
		icmpH->checksum = 0;
		icmpH->checksum = checksum(packet,used+datasize,protocol);
		

		// https://man7.org/linux/man-pages/man3/sendto.3p.html
		if (sendto(fd,packet,used+datasize,0,&servinfo,servlen) < 0){
			//cleaning process TODO
			printErr("Function sendto() failed");
		}
		
		totalBytesSent+=datasize;
		moving_file_ptr = &ptr->filebuff[totalBytesSent];
		// seqNum++;

		// sprintf(seqNum_char,"%d",seqNum);
		// used = icmpHlen+sizeof(seqNum_char); //standard size of headers
		
		// copy sequence number to packet
		// memcpy(packet+icmpHlen,seqNum_char,sizeof(seqNum_char));

	}

	printf("done!\n");


}
