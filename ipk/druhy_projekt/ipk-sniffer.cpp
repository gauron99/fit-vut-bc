/*
 *  C file for IPK project
 *  IPK project #2 option ZETA: Sniffer of packets
 *  author: Fridrich David
*/

//ethernet address is 6 bytes
#define ETHER_ADDR_LEN	6
//eth header is always exactly 14 bytes
#define SIZE_ETHERNET 14

#include<iostream>
#include "ipk-sniffer.h"
#include <time.h>


// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct ether_header *ethernet; //ethernet header
struct iphdr *ip; //ip4 header
struct tcphdr *tcp; //tcp header
struct udphdr *udp; //udp header

const u_char *payload; //packet payload

//ip6
struct ip6_hdr *ip6; //ip6 header
char IP6src[INET_ADDRSTRLEN];
char IP6dst[INET_ADDRSTRLEN];


using namespace std;

void
printHelpMessage(){
    cout << "This is a help message for ipk-sniffer" << endl << endl;
    cout << ">> Possible program arguments are:" << endl;
    cout << "      (-i <interface>) >> interface to listen on, if parameter is not given, list all available devices" << endl;
    cout << "      (-p <port>) >> filter packets on specified device by port, if not given, all ports are considered valid " << endl;
    cout << "      (-t/--tcp) >> show only tcp packets" << endl;
    cout << "      (-u/--udp) >> show only udp packets" << endl;
    cout << "          Note: if neither -u or -t is given, show both" << endl;
    cout << "      (-n <number>) >> defines number of packets to be shown, if not given, one packet is shown" << endl;
    cout << "      (-h/--help) >> prints out help message" << endl;
}


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

// packet_callback_function (called every time when packet is recieved)
void
packet_cb_func(u_char *args, const struct pcap_pkthdr *header,const u_char *packet){

    //convert time to printable form

    char buff2[64];
    struct tm *t2;
    
    // convert time_t header -> tm *t2
    t2 = localtime(&header->ts.tv_sec);
    //convert to printable time (tm *t2 -> char*)
    strftime(buff2,sizeof(buff2),"%H:%M:%S",t2);

    //"append" miliseconds at the end
    // print out format == 'čas IP|FQDN : port > IP|FQDN : port'
    printf("%s.%.6ld ",buff2,header->ts.tv_usec); //print out "time "

	int size_ip;
	int size_tcp;
    int size_udp;
    int payloadLen;
    
    // int nextHead; //for ipv6 only


    //get ethernet part of the packet == is first
    ethernet = (struct ether_header*)(packet);
    //get IP part of the packet (exclude ethernet part)
    ip = (struct iphdr*)(packet + SIZE_ETHERNET);
    /* - Internet Header Length is the length of the internet header in 32 bit words,
     * and thus points to the beginning of the data. Note that the minimum value
     * for a correct header is 5.
     * - Which means whatever value is stored in the IHL, it should be multiplied
     * with 32 to get the total number of bits, or with 4 to get the total number of bytes.
     * - From: 'https://stackoverflow.com/questions/11668172/how-do-i-calculate-ip-header-length'
     */
    size_ip = ip->ihl*4;
    if (size_ip < 20) {
        printf(" Invalid IP header length: %u bytes\n", size_ip);
        exit(1);
    }
    //ip for IPv6
    // int size_ip6 = SIZE_ETHERNET + sizeof(struct ip6_hdr);

    
    int size_header;
    switch (ntohs(ethernet->ether_type))
    {
    case ETHERTYPE_IP: //IPv4
        switch (ip->protocol)
        { //ETHERTYPE_IP == IPv4
        case IPPROTO_TCP: //packet is from TCP IPv4

            tcp = (struct tcphdr*)(packet + SIZE_ETHERNET + size_ip);
            size_tcp = tcp->th_off*4;
            if (size_tcp < 20) {
                printf("Error: Invalid TCP header length of %u bytes\n", size_tcp);
                exit(1);
            }

            //inet_ntoa = internet address manipulation routine.
            //function converts the Internet host address in, given in network byte order,
            // to a string in IPv4 dotted-decimal notation

            //print "IPsrc : port"
            cout << inet_ntoa(*(in_addr*)&ip->saddr);
            cout << " : " << ntohs(tcp->th_sport) << " > "; //print out "time IP : port > "

            //print "IPdest : port"
            cout << inet_ntoa(*(in_addr*)&ip->daddr);
            cout << " : " << ntohs(tcp->th_dport) << endl; //print out "time IPsrc : port > IPdest : port"
            cout << endl;        

            size_header = SIZE_ETHERNET + size_ip + size_tcp;

            
            //asign pointer to payload -> to the beggining of it (aka packet + skip header -> ethernet,ip,tcp info)
            payload = (const u_char *)(packet + size_header);
            //get size of the payload
            payloadLen = ntohs(ip->tot_len) - (size_ip + size_tcp);

            //print out header
            printOut(packet,size_header);
            cout << endl;

            //print out payload
            if (payloadLen > 0){
                printOut(payload,payloadLen);
            }

            break;
        case IPPROTO_UDP: //tcp IPv4

            udp = (struct udphdr*)(packet + SIZE_ETHERNET + size_ip); 
            size_udp = ntohs(udp->len);
            //print "IPsrc : port"
            cout << inet_ntoa(*(in_addr*)&ip->saddr);
            cout << " : " << ntohs(udp->source) << " > "; //print out "time IP : port > "

            //print "IPdest : port"
            cout << inet_ntoa(*(in_addr*)&ip->daddr);
            cout << " : " << ntohs(udp->dest) << endl; //print out "time IPsrc : port > IPdest : port"
            cout << endl;      

            size_header = SIZE_ETHERNET + size_ip + size_udp;
            
            payload = (const u_char *)(packet + size_header);
            //get size of the payload
            payloadLen = ntohs(ip->tot_len) - (size_ip + size_udp);

            //print out header
            printOut(packet,size_header);
            cout << endl;

            //print out payload
            if (payloadLen > 0){
                printOut(payload,payloadLen);
            }

            break;

        default:
            break;
            
        }
            /*
            * ETHERNTYPE_IPV6 part inspired by: 
            * 'https://github.com/yuan901202/ethernet_packet_sniffer/blob/master/eps.c'
            */
        break;
    case ETHERTYPE_IPV6: //IPv6
    //     cout << "IPV BABY" << endl;
    //     ip6 = (struct ip6_hdr*)(packet + SIZE_ETHERNET);

    //     //get ipv6 source & dest -> save into IP6src & IP6dst
    //     inet_ntop(AF_INET6, &(ip6->ip6_src),IP6src, INET6_ADDRSTRLEN);
    //     inet_ntop(AF_INET6, &(ip6->ip6_dst), IP6dst, INET6_ADDRSTRLEN);

    //     nextHead = ip6->ip6_nxt;

    //     switch (nextHead)
    //     {
    //     case IPPROTO_TCP:
    //         cout << " " << IP6src << " " << IP6dst << endl;
            
    //         break;
    //     case IPPROTO_UDP:
    //         cout << " " << IP6src << " " << IP6dst << endl;

    //         break;
    //     default:
    //         break;
    //     }

        break;
    // default:
    //     break;
    }


}

void
printOut(const u_char *buffer, int len){

    static int offset = 0; // beginning part of what is printed out
    int lineMaxWidth = 16; // max number of bytes to fit on one line "middle part of what is printed out", hex
    const u_char *b = buffer;

    //check if length of the data is actually greater than zero, if not, there is nothing to print
    if(len <= 0){
        return;
    }

    /*
     * if the data can fit on one line, we can just use the function print_hex_ascii_line()
     * from 'http://www.tcpdump.org/'
     * to print the payload in offset hex ascii form
     */

    if (len < lineMaxWidth){
        print_hex_ascii_line(b,len,offset);
        return;
    }

    //else -> if data is larger than one line, we must print it line by line

    // with each line total length will be subtracted be printed line
    // to determine when to stop
    int length = len; 
    int currentLen;


    while(true){
        // if()
        //get length of the current line
        currentLen = lineMaxWidth % length;
        length -= currentLen;

        print_hex_ascii_line(b,currentLen,offset);

        //move the pointer to payload by length of the lne printed out
        b += currentLen;
        //move offset by the whole line
        offset += lineMaxWidth;

        //if the length is less than full line (lineMaxWidth), print whats left and exit
        if(length <= lineMaxWidth){
            print_hex_ascii_line(b,length,offset);
            offset += lineMaxWidth;
            break;
        }

    };

}   

// function taken from 'http://www.tcpdump.org/'
void
print_hex_ascii_line(const u_char *buffer, int len, int offset)
{

	int i;
	int gap;
	const u_char *ch;

	/* offset */
	printf("%05x   ", offset);
	
	/* hex */
	ch = buffer;
	for(i = 0; i < len; i++) {
		printf("%02x ", *ch);
		ch++;
		/* print extra space after 8th byte for visual aid */
		if (i == 7)
			printf(" ");
	}
	/* print space to handle line less than 8 bytes */
	if (len < 8)
		printf(" ");
	
	/* fill hex gap with spaces if not full line */
	if (len < 16) {
		gap = 16 - len;
		for (i = 0; i < gap; i++) {
			printf("   ");
		}
	}
	printf("   ");
	
	/* ascii (if printable) */
	ch = buffer;
	for(i = 0; i < len; i++) {
		if (isprint(*ch))
			printf("%c", *ch);
		else
			printf(".");
		ch++;
	}

	printf("\n");

return;
}



int 
main(int argc,  char *const *argv)
{

    //inicialize variable for class used for arguments
    Arguments args;
    int opt = 0;
    int index;

    //structure for longopts (to work with '--' options easier)
    const struct option longopts[] = 
    {
        {"tcp", 0, 0, 't'}, // --tcp == -t
        {"udp", 0, 0, 'u'}, // --udp == -u
        {"help",0, 0, 'h'}, // print out help
        {0,0,0,0},
    };

    char *temp;
    char *err;
    //while-switch to load all program arguments and check if no unsupported args were given
    while((opt = getopt_long(argc,argv,"hi:p:tun:",longopts, &index)) != -1)
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
                strtol(args.p_opp,&err,10);
                if(*err){
                    cout << "Error: <port> "<< temp << " given by -p parameter is not a number. Try --help for help" << endl;
                    return 1;
                }
                break;

            case 't':
                args.t = true;
                break;

            case 'u':
                args.u = true;
                break;

            case 'n':
                args.n = true;
                temp = optarg;
                args.n_opp = (int)strtol(temp,&err,10);
                if(*err){
                    cout << "Error: <number> "<< optarg << " given by -n parameter is not a number. Try --help for help" << endl;
                    return 1;
                }
                break;

            case 'h':
                printHelpMessage();
                return 0;

            case '?':
            default:
                cout << "unkown option: " << optopt << endl;
                cout << "Try -h or --help parameter to display help message" << endl;
                return 1;


        }
        temp = NULL;
        err = NULL;
    }

    //list of devices (loaded in function 'findAllDevices')
    pcap_if_t *interfaceList;

        //function returns false when an error occured, ret true on success
    if(!findAllDevices(&interfaceList)){
        return 1;
    }
    
    // if -i option was not given, list all devices available for sniffing and exit successfully
    if (!args.i){
        int count = 1;
        cout << "Parameter '-i' was not given as an argument, listing devices..." << endl;
        cout << "Available devices are: " << endl;
        pcap_if_t *d = interfaceList; // d = device
        while(d != NULL)
        {
            cout << count << "." << d->name << endl;
            d = d->next;
            count++;
        }
        cout << endl;
        //free live devices
        pcap_freealldevs(interfaceList);
    
        return 0;
    }

    //else start the sniffing


    char errbuf[PCAP_ERRBUF_SIZE];
    char *dev;
    
// ~~~~~~~~~~~~~~~~~ check -i parameter, if given ~~~~~~~~~~~~~~~~~ //
    /*
     * for-loop to check if given interface actually exists, if so, save it in a variable
     * for furter use, if not, print info message and exit with an error
     */
    bool found = false;
    for(pcap_if_t *temp = interfaceList;temp != NULL;temp = temp->next){
        if(!strcmp(args.i_opp,temp->name)){
            found = true;
            dev = temp->name;
        }
    }
    if (!found){
        cout << "Device "<< args.i_opp <<" was not found."
        " You can list all available devices by running ipk-sniffer without any parameters"
        " or use --help to print out a help message" << endl;
        return 1;
    }

    /*
     * all code snippets bellow commented as "tcpdump code snippet" are taken from 'https://www.tcpdump.org/pcap.html'
    */

    pcap_t *handle;
    struct bpf_program fp; // compiled filter expression
    char port_filter[] = ""; // filter expression itself in str format (ex: "udp port 23")
    bpf_u_int32 mask; //netmask for the chosen device
    bpf_u_int32 net; // IP of the device

    // struct pcap_pkthdr header; // header //TODO
    // const u_char *packet; // actual packet 

    //if specific filter was given, concatenate with string and
    //create string as filter for pcap_compile
    

    //--tcp only (not udp)
    if(args.t && (!args.u)){
        strcat(port_filter,"tcp"); // temp = "tcp "
    }
    //udp only (not tcp)
    else if(args.u && (!args.t)){
        strcat(port_filter,"udp"); // temp = "tcp "
    }
    /* else{
        * (both given == none given because 'port 32' == '(udp or tcp) port 32') 
        * therefore args.t && args.u -> take both udp and tcp
        * }
    */
    else{
        strcat(port_filter,"tcp or udp");
    }

    if(args.p){
        // if port is given, concatenate the string too
        strcat(port_filter," ");
        
        strcat(port_filter,"port "); //add 'port ' at the end of string filter
        strcat(port_filter,args.p_opp); //add the actual port -> string is for examle: "udp port 53"
    }
    
    cout <<"Filter: '"<< port_filter <<"'" << endl; //debug
    cout << endl;


    if(pcap_lookupnet(dev,&net,&mask,errbuf) == -1){
        cerr << "Can't get netmask for "<< dev << " -> set to 0" << endl;
        net =0;
        mask =0; 
    }

    //tcpdump code snippet
    handle = pcap_open_live(dev,BUFSIZ,true,1000,errbuf);
    if(handle == NULL){
        cerr << "Error, device " << dev <<" couldn't be opened -> " << errbuf << endl;
        return 2;
    }

    //if any filter is set, apply the filter
    if(args.p || args.t || args.u){
       
        //tcpdump code snippet
        if(pcap_compile(handle,&fp,port_filter,0,net) == -1){
            cerr << "Error in parsing the filter (pcap_compile) " << port_filter <<": " << pcap_geterr(handle) << endl;
            return 2;
        }
        // cout << "Done pcap_compile" << endl;
        //tcpdump code snippet
        if(pcap_setfilter(handle, &fp) == -1) {
                fprintf(stderr, "Couldn't install filter %s: %s\n", port_filter, pcap_geterr(handle));
                return 2;
        }
    }
    // list of all the devices is not needeed anymore, free it
    pcap_freealldevs(interfaceList);

    /* pcap_loop()
     * handle = like file handle..."session" so we can differentiate between all other sessions (from pcap_open_live)
     * args.n_opp = number of packets to be sniffed (default is one)
     * if args.n_opp is negative, sniff endlessly 
     */
    pcap_loop(handle,args.n_opp,packet_cb_func,NULL);


    pcap_freecode(&fp);
    pcap_close(handle);

    
    return 0;
}









