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

#include <pcap/pcap.h>
#include <sys/stat.h> //file
#include <sys/types.h>

#include <netinet/ip_icmp.h>	//icmphdr, ICMP_ECHO

#include <openssl/aes.h>// encryption

#include <signal.h>
// 1385
#define PACKET_MAX_SIZE   1425          //max size for packet (+ ip hdr + 15 padding)
#define DATA_MAX_SIZE     1400          //max data size in packet
#define MAX_FILE_LEN      1099511627775 // 1 TiB
#define FILE_LEN_BYTES    13            //(strlen(1099511627775) = 13 positions = 1TiB as int)
#define MAX_FILE_NAME_LEN 3             //number of bytes needed to write file name (max 999 string)
#define SIZE_LCC          16            // size of Linux Cooked Capture (when listening on "any")
#define PACKET_ID         9578          // ID of every packet sent by this program

//encryption key used in encryption and decryption
extern const char *encryptionKey;

typedef struct settings{
  char          *file_name;
  unsigned char *filebuff;
  FILE          *f;

  int           is_server;
  int           verbose_printing;
}settings;

extern settings *ptr;
extern pcap_t *handler;

/**
 * function initializes structure 'settings' values for the program
 */
void init();

/**
 * function frees all memory inside program structure 'settings' that needs to be freed
 */
void cleanStruct();

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
 * function checks whether file is empty or not
 * @param file path to file
 * @return returns True if is empty, False otherwise
 */
int
fileIsEmpty(char *file);

/**
 * function starts a server, listen for ICMP incoming traffic, decodes the 
 * messages and subsequently saves a file transfered by ICMP packets to folder
 * where program is started in 
 */
void
server();

/**
 * function creates a client, who connects to given server {@code host} 
 * and sends ICMP packets with encrypted {@code file}.
 * @param file a file to transfer to server through ICMP encrypted packets
 * @param host ip or hostname of server to connect to
 */
void
client(char *file, char *host);

/**
 * function encrypts data pointed to by 'in' with length of 'len' within 16B blocks
 * @param in pointer where the data is 
 * @param len length of data to be encrypted
 * @return returns pointer to encrypted data 
 */
unsigned char
*encryptData(char *in,unsigned int *len);


/**
 * function dencrypts data pointed to by 'd' with length of PACKET_MAX_DATA
 * @param d pointer where the encrypted data is 
 * @return returns pointer to dencrypted data 
 */
unsigned char
*decryptData(const unsigned char *d);

/**
 * function calculates max free space dependent on headers/info in packet already
 * @param used how much data is already used in packet
 * @param done how much data(bytes) of file already transfered/already read
 * @param fl total length of file being transfered
 * @return returns available length
 */
int
getMaxDataAvailable(int used,int done, int fl);

/**
 * function creates first packet (all file info necessary for the transfer)
 * @param p pointer to packet where the data is to be written
 * @param used says how much data is already written in packet
 * @param l total length of whole file
 * @return how much data is used in packet after info is copied in
 */
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
 * function takes in full/relative path as parameter and returns file name
 * AKA it finds the last "/" and gets rid of everything before it including "/"
 * @param path to file
 * @return returns extracted file name
 */
char
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
parser(int argc, char **argv, char **file,char **host);

/**
 * calculate checksum of given packet - works for IPv4
 * @param b packet pointer
 * @param l length of data
 * @param p protocol used in order to check IPV4
 * @return returns checksum value
 */
unsigned short
checksum(void *b,int l,int p);

/**
 * callback function for pcap_loop
 * handles all incoming packets
 * @param args arguments passed to cb
 * @param header contains info about header (length etc.)
 * @param packet pointer to actual packet sniffed
 */
void 
packet_hdlr_cb(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

/**
 * extract file name from packet, server side
 * @param p pointer to packet
 * @param r how many bytes were read after this extraction (move pointer after this)
 */
void
getFilenameFromPacket(char *p, unsigned int *r);


/**
 * get file length from packet
 * this is done subsequently after getFilenameFromPacket since its right after in
 * the actual packet
 * @param p pointer to packet
 * @param r how many byts were read (before + updated during since it takes a pointer)
 * @return returns length got from packet (of file)
 */
unsigned int
getFileLenFromPacket(char *p, unsigned int *r);

/**
 * function prepares output for file
 * checks whether said file exists and is readable and writable. IF not create
 * a backup file with "isa_" as prefix (this file is not checked for existence,
 * its overwritten if necessary)
 * function works with global 'settings' structure
 */
void
prepOutputFile();

/**
 * function handles SIGINT signal
 * When CTRL+C (SIGINT) signal is sent it frees some memory before exiting
 */
void
sigint_handler(int sn);

#endif //ISA_SECRET_H