// client.cpp
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling

#include "client.h"
#include "string.h"
using namespace std;

Client :: Client() {
}

void Client :: connect_to_server(char* hostname, int port) {
    struct hostent *hp;
    
    // Translate host name into IP address
    hp = gethostbyname(hostname);
    if (!hp) {
        cerr << "myfrm: unknown host: " << hostname << endl;
        exit(1);
    }

    // Build socket-in address
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(port);

    // Open UDP Socket
    if ((udp_s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "myfrm: udp socket" << endl;
        exit(1);
    }

    // Open TCP Socket
    if ((tcp_s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "myfrm: tcp socket" << endl;
        exit(1);
    }

    // Connect to server
    if (connect(tcp_s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        cerr << "myfrm: connect\n" << endl;
        close(tcp_s);
        exit(1);
    }
}

void Client :: send_input() {
    string command;

    while (cin >> command) {        
        if (command == "CRT") {
        } else if (command == "MSG") {
        } else if (command == "DLT") {
        } else if (command == "EDT") {
        } else if (command == "LIS") {
        } else if (command == "RDB") {
        } else if (command == "APN") {
        } else if (command == "DWN") {
        } else if (command == "DST") {
        } else if (command == "XIT") {
        } else if (command == "SHT") {
        } else {
            //cout << "Invalid Operation" << endl;
        }
        
        int len = command.length();
        if (sendto(udp_s, command.c_str(), len, 0, (struct sockaddr*) &sin, sizeof(struct sockaddr)) == -1) {
            cerr << "Client send error" << endl;
            exit(1);
        } 
    }
}

void Client :: print_commands() {
    cout << "Enter one of the following commands:" << endl;
    cout << "CRT: Create Board \nMSG: Leave Message" << endl;
}

