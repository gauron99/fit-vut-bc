/*
**  IPK projekt #2 Varianta ZETA: Sniffer paketu
**  autor: Fridrich David
**  program: good boi sniffer gon be sniffin som good stuff
*/

#include<iostream>

#include<libnet.h> //ze zadani, pro implementaci
#include<pcap.h>


using namespace std;
 
int 
main(int argc,  char* const* argv)
{
    char a[4];
    cout << ">> You want me to sniff? " << endl;
    cin >> a;
    if (strcmp(a, "yes") || strcmp(a ,"Yes") || strcmp(a, "y") || strcmp(a, "Y"))
    {
        cout << ">> Alriight here i go\n" << endl;
    }
    else
    {
        cout << "Oh okay ill wait then" << endl;
        exit(0);
    }

/*
*   arguments: -i[eth0], -p[23], -t(--tcp), -u(--udp), -n [10] 
*   [] means that particular option requires an argument to follow (like -i eth0)
*/


int opt = 0;
char *i_opt;

    while((opt = getopt(argc,argv,"i:p:tun:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                i_opt = optarg;
                break;
            case 'p':
                cout << "hello im p: " << optarg << endl;
                break;
            case 't':
                cout << "hello im t option: " << endl;
                break;
            case 'u':
                cout << "ey mi u option: " << endl;
                break;
            case 'n':
                cout << "im n yo: " << optarg << endl;
                break;
            case '?':
                cout << "unkown option bud: " << optopt << endl;
                break;
            default:
                cout << "default something i guess: " << opt << endl;
                break;
        }
    }


    //finding all devices available

    int err;
    int count = 1;
    pcap_if_t *interfaceList;
    char errbuf[PCAP_ERRBUF_SIZE];
    cout << "Finding all available devices... ";
    err = pcap_findalldevs(&interfaceList,errbuf);
    if (err < 0) //error
    {
        cout << "Error while searching for list n stuff: " << errbuf << endl;
        return 1;
    }
    cout << "Done" << endl;

    cout << "Available devices are: " << endl;
    pcap_if_t *device = interfaceList;
    while(device != NULL)
    {
        cout << count << "." << device->name << endl;

        count++;
        device = device->next;
    }
    cout << endl;
    //free live devices
    pcap_freealldevs(interfaceList);

    return 0;
}









