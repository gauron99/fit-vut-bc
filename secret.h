//secret.h

////////////////////////////////////////////////////////////////////////////////
//  Used for ISA project @BUT_FIT (cz:FIT VUT)                                //
//  author: David Fridrich                                                    //
//  file: secret.h - declaration of functions used by secret.c                //
//  last updated: 03.11.2021                                                  //
////////////////////////////////////////////////////////////////////////////////

// guards
#ifndef ISA_SECRET_H
#define ISA_SECRET_H

#include <stdio.h>

typedef struct settings{
  int verbose_printing;
  char *file_name;

  int is_server;
}settings;

settings *ptr;

void init_settings();

void clean_settings();

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

/**
 * function creates a client, who connects to given server {@code host} 
 * and sends ICMP packets with encrypted {@code file}.
 * @param file a file to transfer to server through ICMP encrypted packets
 * @param host ip or hostname of server to connect to
 */
void
client(char *file, char *host);



/**
 * function checks if a file {@code file} provided as a path of this file exists.
 * If it does, function executes normally and returns nothing, if file doesn't
 * exist, function prints out an error and exits immediately.
 * @param file pointer to a "string" containing a path to a file
*/
void
fileExists(char *file);


/**
 * function is called on already existing file (no existing check is done here)
 * it opens the file, reads its contents as bytes and returns the file pointer.
 * @param in path to the existing file
 * @param len (pointer to) length of this file in bytes
 * @param filename name of the file being sent(taken from provided path)
 * @returns malloced array of bytes with file content (+ length in bytes via parameter {@code len})
*/
char
*fileOpenReadBytes(char *in, unsigned long int *len, char **filename);


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

unsigned short
checksum(void *b,int l,int p);

void
packet_hdlr_cb(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);


#endif //ISA_SECRET_H