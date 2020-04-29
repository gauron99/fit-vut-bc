/*
 *  C file for IPK project
 *  IPK project #2 option ZETA: Sniffer of packets
 *  author: Fridrich David
*/

#include<iostream>
#include "proj.h"

using namespace std;


bool 
findAllDevices(pcap_if_t **interfaceList){
    int err;
    
    char errbuf[PCAP_ERRBUF_SIZE];
    cout << "Finding all available devices... ";
    err = pcap_findalldevs(&(*interfaceList),errbuf);
    if (err < 0) //error
    {
        cout << "Error in function: '" << __FUNCTION__ <<"' while searching for possible devices to sniff on: " << errbuf << endl;
        return false;
    }
    cout << "Done" << endl;

    return true;
}




int 
main(int argc,  char *const *argv)
{
    char a[4];
    cout << ">> You want me to sniff? " << endl;
    cin >> a;
    if (strcmp(a, "yes") || strcmp(a ,"Yes") || strcmp(a, "y") || strcmp(a, "Y") || strcmp(a, "Ye") || strcmp(a, "ye"))
    {
        cout << ">> Alriight here i go\n" << endl;
    }
    else
    {
        cout << "Oh okay ill end then" << endl;
        exit(0);
    }

    
    Arguments args;

    int opt = 0;
    int index;

    //structure for longopts (to work with '--' options easier)
    const struct option longopts[] = 
    {
        {"tcp", 0, 0, 't'}, // --tcp == -t
        {"udp", 0, 0, 'u'}, // --udp == -u
        {0,0,0,0},
    };


    while((opt = getopt_long(argc,argv,"i:p:tun:",longopts, &index)) != -1)
    {
        switch (opt)
        {
            case 'i':
                args.i = true;
                args.i_opp = optarg;
                break;
            case 'p':
                args.p = true;
                args.p_opp = optarg;
                cout << "hello im p: " << optarg << endl;
                break;
            case 't':
                args.t = true;
                cout << "hello im t option: " << endl;
                break;
            case 'u':
                args.u = true;
                cout << "ey mi u option: " << endl;
                break;
            case 'n':
                args.n = true;
                args.n_opp = optarg;
                cout << "im n yo: " << optarg << endl;
                break;
            case '?':
                cout << "unkown option bud: " << optopt << endl;
                exit(1);
            default:
                cout << "default something i guess: " << opt << endl;
                exit(1);

        }
    }
    //list of devices (loaded in function 'findAllDevices')
    pcap_if_t *interfaceList;

        //function returns false when an error occured, ret true on success
    if(!findAllDevices(&interfaceList)){
        return 1;
    }
    
    // if -i option was not given, list all devices available for sniffing exit
    if (!args.i){
        int count = 1;

        cout << "Available devices are: " << endl;
        pcap_if_t *device = interfaceList;
        while(device != NULL)
        {
            cout << count << "." << device->name << endl;
            device = device->next;
            count++;
        }
        cout << endl;
        //free live devices
        pcap_freealldevs(interfaceList);
    
        return 0;
    }

    //else start the sniffing

    cout << "Starting to sniff..." << endl;
    char errbuf[PCAP_ERRBUF_SIZE];
    char *dev = interfaceList->next->name;

    /*
     * code snippets taken from 'https://www.tcpdump.org/pcap.html'
    */

    pcap_t *handle;
    struct bpf_program fp; // compiled filter expression
    char port_filter[] = "port 23"; // filter expression itself in str format (ex: "port 23")
    bpf_u_int32 net;

    struct pcap_pkthdr header; // The header that pcap gives us 
    const u_char *packet; // The actual packet 


    handle = pcap_open_live(dev,BUFSIZ,true,1000,errbuf);
    if(handle == NULL){
        cerr << "Error, device " << dev <<" couldn't be opened -> " << errbuf << endl;
        return 2;
    }
    cout << "Done pcap_open_live" << endl;
    if(pcap_compile(handle,&fp,port_filter,0,net) == -1){
        cerr << "Error in parsing the filter (pcap_compile) " << port_filter <<": " << pcap_geterr(handle) << endl;
        return 2;
    }
    // cout << "Done pcap_compile" << endl;
    // if(pcap_setfilter(handle, &fp) == -1) {
	// 		fprintf(stderr, "Couldn't install filter %s: %s\n", port_filter, pcap_geterr(handle));
	// 		return 2;
	// }
    // cout << "Done pcap_setfilter" << endl;

    packet = pcap_next(handle,&header);
    printf("Jacked a packet with length of [%d]\n", header.len);

    pcap_close(handle);

    pcap_freealldevs(interfaceList);
    
    return 0;
}









