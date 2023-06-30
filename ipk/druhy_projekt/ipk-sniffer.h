/*
 *  header file for IPK project #2 
 *  theme: Packet Sniffer
 *  Author: David Fridrich
 *  
*/

#ifndef SNIFFER_H
#define SNIFFER_H

    // includes

    #include <getopt.h>

    #include<libnet.h>
    #include<pcap.h>

    #include <netinet/ip.h> //ipv4 header
    #include <netinet/tcp.h> //tcp header
    #include <netinet/udp.h> //udp header
    #include <netinet/if_ether.h> //ethernet header
    #include <netinet/ether.h>  //ethernet header declarations
    #include <netinet/ip6.h>

    //class

    /*
     *  arguments: -i[eth0], -p[23], -t(--tcp), -u(--udp), -n [10] 
     *  [] means that particular option requires an argument to follow (like -i eth0)
     *  -i -> interface to sniff on
     *      -- if '-i' is not given, list all possible interfaces
     * 
    */

    /*  
     * bool values indicate given arguments (set true if option has been passed in cmd)  
     * some options take additional argument ( i, p, n). Each of them has additional
     * argument of type char* to which the followed argument will be saved into
    */

    class Arguments
    { public:

        bool i = false; 
        char *i_opp = {0};
        
        bool p = false;
        char* p_opp = {0};
        
        bool t = false;

        bool u = false;
        
        bool n = false;
        int n_opp = 1;

    };


    //functions


    /*
     * function is called when one of arguments is '-h' or '--help'
     * simply prints out a help message how to run the program, what are
     * specific arguments and how to use them 
     */
    void printHelpMessage();

    /*
     * -- is called only when '-i' parameter is not passed via
     * program arguments.
     * -- finds all available devices for sniffing and prints them out
     * 
     * @ret return true on success, return false if an error occured
     */
    bool findAllDevices(pcap_if_t *interfaceList);

    /*
     * callback function. is called whenever a packet is sniffed by pcap_loop
     * @params 
     *  -- header - contains info such as length of the packet and time stamp
     *  -- packet - is a pointer to an actual info the packet contains
     */
    void packet_cb_func(u_char *args, const struct pcap_pkthdr *header,const u_char *packet);


    /*
    * authorization of the functions bellow:
    * 
    * -- from sniffex.c
    *
    * Sniffer example of TCP/IP packet capture using libpcap.
    * 
    * Version 0.1.1 (2005-07-05)
    * Copyright (c) 2005 The Tcpdump Group
    *
    * This software is intended to be used as a practical example and 
    * demonstration of the libpcap library; available at:
    * http://www.tcpdump.org/
    *
    ****************************************************************************
    *
    * This software is a modification of Tim Carstens' "sniffer.c"
    * demonstration source code, released as follows:
    * 
    * sniffer.c
    * Copyright (c) 2002 Tim Carstens
    * 2002-01-07
    * Demonstration of using libpcap
    * timcarst -at- yahoo -dot- com
    */

   // function for printing out specific characters hex numbers, ASCII characters
    void print_hex_ascii_line(const u_char *buffer, int len, int offset);

    // function for printing out data of the packet in the right format (offset - hex - ASCII)
    void printOut(const u_char *buffer, int len);

#endif