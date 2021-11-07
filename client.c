
#include "secret.h"



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
	char *filebuff = fileOpenReadBytes(file,&filelen);

	if (filelen == 0){
		free(ptr->file_name); // TODO resolve this bad boy
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
		printf("addr:%s\n",target->ai_addr->sa_family==2?"ipv4":"ipv6 or other");
		printf("family:%s\n",target->ai_family==2?"ipv4":"icmp6 or other");
		if(protocol != 1){
			printf("protocol:%s(val: %d)\n", "ICMP6 maybe?",protocol);
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
	 * PACKET
	 * [0-7] -- icmp header
	 * [8-9]32bit / [8-11]64bit -- sequence number
	 * 
	 * first packet: 
	 * 	    [12-16] -- file name length
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
	icmpH->type = ICMP_ECHO;

	// max value is a number that fits in 5 bytes
	char filelen_char[FILE_LEN_BYTES];
	sprintf(filelen_char,"%ld",filelen); //done DEBUG

	printf("filelen num (%ld)\n",filelen); //DEBUG
	printf("strlen(filelen_char) alloc'd (%ld)\n",strlen(filelen_char)); //DEBUG

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
	memcpy(packet+used,ptr->file_name,strlen(ptr->file_name));
	used += strlen(ptr->file_name);

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
		
		//encryption TODO
		
		// copy file data
		memcpy(packet+used,moving_file_ptr,datasize);

		//calculate checksum
		icmpH->checksum = 0;
		icmpH->checksum = checksum(packet,used+datasize,protocol);
		
		// https://man7.org/linux/man-pages/man3/sendto.3p.html
		if (sendto(fd,packet,used+datasize,0,&servinfo,servlen) < 0){
			//cleaning process TODO
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

	printf("done!\n");


}
