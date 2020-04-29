/*
 *  header file for IPK project #2 
 *  theme: Packet Sniffer
 *  Author: David Fridrich
 *  
*/

#ifndef PROJ_H
#define PROJ_H

    // includes
    #include <getopt.h>

    #include<libnet.h> //ze zadani, pro implementaci
    #include<pcap.h>


    //classes

    /*
     *  arguments: -i[eth0], -p[23], -t(--tcp), -u(--udp), -n [10] 
     *  [] means that particular option requires an argument to follow (like -i eth0)
     *  -i -> interface to sniff on
     *      -- if '-i' is not given, list all possible interfaces
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
        char *p_opp = {0};
        
        bool t = false;

        bool u = false;
        
        bool n = false;
        char *n_opp = {0};
    };


    //functions

    /*
     * -- is called only when '-i' parameter is not passed via
     * program arguments.
     * -- finds all available devices for sniffing and prints them out
     * 
     * @ret return true on success, return false if an error occured
     */
    bool findAllDevices(pcap_if_t *interfaceList);

#endif