
#include "secret.h"


extern struct settings *ptr;

int getMaxDataAvailable(int used,int done,int fl){
  return (fl-done) < (PACKET_MAX_SIZE-32-used) ? (fl-done) : (PACKET_MAX_SIZE-32-used);
}

int createFirstPacket(char (*p)[PACKET_MAX_SIZE], int used, unsigned int l){

	unsigned int fnsi = strlen(ptr->file_name);
  if(fnsi>999){
    printErr("Length of file name is larger than allowed (max=999 chars)");
  } else {
    char fns[MAX_FILE_NAME_LEN] = {0};
    if(sprintf(fns,"%u",fnsi) < 0){
      printErr("Sprintf() failed @createFirstPacket()");
    }
		//copy length of name to packet
    memcpy(*p + used,fns,MAX_FILE_NAME_LEN);
    used += MAX_FILE_NAME_LEN;
  }

  // copy file name to packet
	memcpy(*p + used,ptr->file_name,fnsi);
	used += fnsi;

	// max value is a number that fits in 13 bytes
	char filelen_char[FILE_LEN_BYTES] = {0};
	sprintf(filelen_char,"%u",l);

	// copy file length to packet
	memcpy(*p + used,filelen_char,FILE_LEN_BYTES);
	used += FILE_LEN_BYTES;

  return used;
}

unsigned char *encryptData(char *in,unsigned int *l){
	
	extern const char* encryptionKey;

	printf("finalLen bef enc:%d\n",*l);
	while(((*l)%AES_BLOCK_SIZE)!=0){(*l)++;}
	printf("finalLen after enc:%d\n---\n",*l);

  AES_KEY key;
	AES_set_encrypt_key((unsigned char*)encryptionKey,128,&key);

  unsigned char *res = calloc(*l,1);
  for (int i = 0; i < *l; i += AES_BLOCK_SIZE ){
    AES_encrypt((unsigned char*)in+i,res+i,&key);
  }

  return res;
}


FILE *fileOpenReady(char *in, long unsigned int *len){

	// code snippet to open file in binary mode
	//https://stackoverflow.com/a/22059317
	FILE *f = fopen(in,"rb"); //open in binary mode
	fseek(f,0,SEEK_END);

	long long unsigned int tmp = ftell(f);
	if(sizeof(*len) == 8){ //64bit OS
		if(tmp > MAX_FILE_LEN){
			free(in);
			fclose(f);
			printErr("Size of file exceeds MAX_FILE_LEN (1 TiB) - what are you trying to send, NASA TOP SECRET DOCUMENTS?");
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
	ptr->filebuff = malloc(1); //ready for realloc
	return f;
}

void readFileBytes(unsigned int l){

	ptr->filebuff = realloc(ptr->filebuff,l);
	if(!ptr->filebuff){
		printErr("realloc failed @readFileBytes()");
	}

	// read {1} chunk of data with length {*len} bytes	
	if( fread(ptr->filebuff,l,1,ptr->f) != 1){
		printErr("fread() failed[secret.c -> char *readFileBytes()]");
	}

	return;
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

	ptr->f = fileOpenReady(file,&filelen);

	ptr->file_name = getFilenameFromPath(file);
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
	 * [8-10]     -- file_name length -> X
   * [11-(11+X)]-- file_name
	 * [X-(X+13)] -- file length (MAX_FILE_LEN in secret.h)
   * [after]		-- data
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
  // icmpH->un.frag.mtu = 1500;
  icmpH->un.echo.sequence = htons(1);
  icmpH->un.echo.id = htons(PACKET_ID);

	unsigned int totalBytesSent = 0;
	unsigned int datasize = 0;

  used = createFirstPacket(&packet,icmpHlen,filelen);

  datasize = getMaxDataAvailable(used,totalBytesSent,filelen);

	readFileBytes(datasize);

	//packet holds all info to be sent
	memcpy(packet+used,ptr->filebuff,datasize);
  totalBytesSent = datasize; //save how much data was read from buffer already

	datasize += used - icmpHlen;

	for (int i = 0; i < datasize; i++)
	{
		printf("%c",packet[icmpHlen+i]);
	}
	printf("\n");


  // encrypt data
  unsigned char *enc_data = encryptData(packet+icmpHlen,&datasize);

	// printf("ENCNCNC:\n");
	// int k,l;
	// for (k = 0; k < datasize; k+=AES_BLOCK_SIZE)
	// {
	// 	for (l = 0; l < AES_BLOCK_SIZE; l++)
	// 	{
	// 		printf("%X ",enc_packet[8+k+l]);
	// 	}
	// 	printf("\n");
	// }
	// printf("i:%d\n",k);
		

	// printf("decrpy encrypted:\n");
	// unsigned char* decrr = decryptData(enc_packet);

	// for (int i = 0; i < PACKET_MAX_SIZE-8; i++)
	// {
	// 	printf("%c",decrr[i]);
	// }
	

	//zero out packet after header
	memset(packet+icmpHlen,0,datasize);
	
	//copy encrypted info into packet
	memcpy(packet+icmpHlen,enc_data,datasize);
  free(enc_data);
	
  icmpH->checksum = checksum(packet,icmpHlen+datasize,protocol);

  //send first packet with additional information
  if(sendto(fd,packet,icmpHlen+datasize,0,&servinfo,servlen) < 0){
    printErr("sendto() failed for the first packet");
  } else {  
    icmpH->un.echo.sequence += htons(1);
  }

	used = icmpHlen;

	//send packets until whole file is sent
	while(totalBytesSent<filelen) {
		printf("totalBytesSent:%d,filelen:%ld\n",totalBytesSent,filelen);
		printf("datasize:%d\n",datasize);
		printf("sqn:%d\n",ntohs(icmpH->un.echo.sequence));
		
		datasize = getMaxDataAvailable(used,totalBytesSent,filelen);
		
		readFileBytes(datasize);
		memcpy(packet+used,ptr->filebuff,datasize);

		// encrypt data
  	enc_data = encryptData(packet+icmpHlen,&datasize);

		//zero out packet after header
		memset(packet+icmpHlen,0,PACKET_MAX_SIZE-icmpHlen);
		
		//copy encrypted info into packet
		memcpy(packet+icmpHlen,enc_data,datasize);
  	free(enc_data);
		
		//calculate checksum
		icmpH->checksum = 0;
		icmpH->checksum = checksum(packet,used+datasize,protocol);
		
		// https://man7.org/linux/man-pages/man3/sendto.3p.html
		if (sendto(fd,packet,used+datasize,0,&servinfo,servlen) < 0){
			//cleaning process TODO
			printErr("Function sendto() failed");
		} else {  
    icmpH->un.echo.sequence += htons(1);
  	}
		
		totalBytesSent+=datasize;
	}

	printf("file transfer done!\n");
}
