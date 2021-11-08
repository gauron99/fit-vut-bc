//secret.h

////////////////////////////////////////////////////////////////////////////////
//  Used for ISA project @BUT_FIT (cz:FIT VUT)                                //
//  author: David Fridrich                                                    //
//  file: secret.h - declaration of functions used by secret.c                //
//  last updated: 05.11.2021                                                  //
////////////////////////////////////////////////////////////////////////////////

// guards
#ifndef ISA_SECRET_H
#define ISA_SECRET_H 1


#include <stdio.h>

#include <stdint.h>
#include <stdlib.h> 					// exit()
#include <string.h>						// memset, memcpy

#include <netdb.h>
#include <unistd.h> 					// close()

#include <sys/stat.h> //file
#include <sys/types.h>

#include <netinet/ip_icmp.h>	//icmphdr, ICMP_ECHO

#include <openssl/aes.h>// encryption



//define some constants (packet max size, max file len in bytes, max file len)
#define PACKET_MAX_SIZE 1480
#define MAX_FILE_LEN 1099511627775  // 1 TiB
#define FILE_LEN_BYTES 13           //(strlen(1099511627775) = 13 positions = 1TiB as int)
#define MAX_FILE_NAME_LEN 3         //number of bytes needed to write file name (max 999 string)
#define SIZE_LCC 16                 // size of Linux Cooked Capture (when listening on "any")
#define PACKET_ID 9578              // ID of every packet sent by this program

extern const unsigned char *encryptionKey;

typedef struct settings{
  char *file_name;
  unsigned char *filebuff;

  int verbose_printing;
}settings;

extern settings *ptr;

void init();

void clean();

/**
 * function prints given error string and exits with errcode 1.
 * @param s string given message to be printed out to stderr
 */
void
printErr(char *s);

/**
 * function prints out whole help message (when wrong parameters are given etc.)
 * and exits with errcode 0.
 */
void
printHelp();

/**
 * function starts a server, listen for ICMP incoming traffic, decodes the 
 * messages and subsequently saves a file transfered by ICMP packets to folder
 * where program is started in 
 */
void
server();

void
handleFirstPacket(const u_char* data,struct icmphdr *icmpH,unsigned int sn);


/**
 * function creates a client, who connects to given server {@code host} 
 * and sends ICMP packets with encrypted {@code file}.
 * @param file a file to transfer to server through ICMP encrypted packets
 * @param host ip or hostname of server to connect to
 */
void
client(char *file, char *host);

unsigned char
*encryptData(char *in,unsigned int *len);

unsigned char
*decryptData(const unsigned char *d);

int
getMaxDataAvailable(int used,int done, int fl);

int
createFirstPacket(char (*p)[PACKET_MAX_SIZE], int used, unsigned int l);


/**
 * function checks if a file {@code file} provided as a path of this file exists.
 * If it does, function returns 1 (true), otherwise 0 (false).
 * @param file pointer to a "string" containing a path to a file
 * @return returns 1(true) on success(=file exists), otherwise 0 (false)
*/
int
fileExists(char *file);


/**
 * function is called on already existing file 
 * it opens the file, reads its contents as bytes and returns the file pointer.
 * @param in path to the existing file
 * @param len (pointer to) length of this file in bytes
 * @returns malloced array of bytes with file content (+ length in bytes via parameter {@code len})
*/
unsigned char
*fileOpenReadBytes(char *in, unsigned long int *len);

char //done
*getFilenameFromPath(char *file);
/**
 * function checks & processes cli parameters. returns if client or server is 
 * desired by user(ex: if -l param is given -> server) and if client parameters
 * are given, it asigns them to given pointers 'file' and 'host' and returns
 * them by these arguments
 * @param argc standard argc (in main)
 * @param argv standard argv (in main)
 * @param file string with a file name to be send to server; double 
 * pointer(so value can be returned)
 * @param host string as ip adress/hostname of server to connect to
 * @return returns enum value that dictates if CLIENT or SERVER is to be created
 */
int
parser(int argc, char **argv, char **file,char **host); //done

unsigned short //done
checksum(void *b,int l,int p);

void //done
packet_hdlr_cb(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

char
*getFilenameFromPacket(char *p, unsigned int *r); //done

unsigned int
getFileLenFromPacket(char *p, unsigned int *r); //done

#endif //ISA_SECRET_H