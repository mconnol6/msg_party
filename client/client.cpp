// client.cpp
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling

#include "client.h"
#include "string.h"
#include <cstdio>
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

    // Build socket-in address for udp
    bzero((char *)&udp_sin, sizeof(udp_sin));
    udp_sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&udp_sin.sin_addr, hp->h_length);
    udp_sin.sin_port = htons(port);
    
    // Build socket-in address for tcp
    bzero((char *)&tcp_sin, sizeof(tcp_sin));
    tcp_sin.sin_family = PF_INET;
    bcopy(hp->h_addr, (char *)&tcp_sin.sin_addr, hp->h_length);
    tcp_sin.sin_port = htons(port);

    // Open UDP Socket
    if ((udp_s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "myfrm: udp socket" << endl;
        exit(1);
    }

    // Open TCP Socket
    if ((tcp_s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "myfrm: tcp socket" << endl;
        exit(1);
    }

    // Connect to server
    if (connect(tcp_s, (struct sockaddr *)&tcp_sin, sizeof(tcp_sin)) < 0) {
        cerr << "myfrm: connect\n" << endl;
        close(tcp_s);
        exit(1);
    }

    addr_len = sizeof(udp_sin);
}

bool Client :: send_udp_string(string str) {
    if (sendto(udp_s, str.c_str(), str.length(), 0, (struct sockaddr*) &udp_sin, sizeof(struct sockaddr_in)) == -1) {
        cerr << "Client send error" << endl;
        exit(1);
    }
}

void Client :: send_udp_int(int i) {
    i = htonl(i);
    if (sendto(udp_s, &i, sizeof(int), 0, (struct sockaddr *)&udp_sin, sizeof(struct sockaddr_in)) == -1) {
        cerr << "Client send error" << endl;
        exit(1);
    }
}

void Client :: ack() {
    send_udp_int(1);
}

string Client :: receive_udp_string() {
    char buf[4096];

    if (recvfrom(udp_s, buf, sizeof(buf), 0, (struct sockaddr *)&udp_sin, (socklen_t *)&addr_len) == -1) {
        cerr << "Client receive error" << endl;
        exit(1);
    }
    string str(buf);
    return str;
}

int Client :: receive_udp_int() {
    int i;
    if (recvfrom(udp_s, &i, sizeof(i), 0, (struct sockaddr *)&udp_sin, (socklen_t *)&addr_len) == -1) {
        cerr << "Client receive error" << endl;
        exit(1);
    }
    i = ntohl(i);

    return i;
}

void Client :: send_input() {
    string command;

    bool cont = true;
    while (cont) {        
        print_commands();
        cin >> command;
        if (command == "CRT") {
        } else if (command == "MSG") {
        } else if (command == "DLT") {
        } else if (command == "EDT") {
        } else if (command == "LIS") {
            list_boards();
        } else if (command == "RDB") {
            read_board();
        } else if (command == "APN") {
        } else if (command == "DWN") {
        } else if (command == "DST") {
        } else if (command == "XIT") {
            send_udp_string("XIT");
            close_sockets();
            cont = false;
        } else if (command == "SHT") {
            cont = shutdwn();
        } else {
            cout << "Invalid Operation" << endl;
        }
    }
}

void Client :: print_commands() {
    cout << "\nEnter one of the following commands:" << endl;
    cout << "CRT: Create Board \tMSG: Leave Message" << endl;
    cout << "DLT: Delete Message \tEDT: Edit Message" << endl;
    cout << "LIS: List Boards \tRDB: Read Board" << endl;
    cout << "APN: Append File \tDWN: Download File" << endl;
    cout << "DST: Destroy Board \tXIT: Exit Client \nSHT: Shutdown Server\n" << endl;
    cout << "Enter your command: ";
}

void Client :: close_sockets() {
    close(tcp_s);
    close(udp_s);
}

void Client :: read_board() {
    string board;
    
    send_udp_string("RDB");
    cout << "Enter name of the board to read: ";
    cin >> board;
    send_udp_string(board);

    int size = receive_udp_int();
    int len, recvd = 0;
    char buf[4096];
    if (size < 0) {
        cout << "Not a valid board!" << endl;
    } else {
        while (recvd < size) {
            bzero(buf, sizeof(buf));
            if ((len = recv(tcp_s, buf, sizeof(buf), 0)) == -1) {
                cerr << "Receive error!" << endl;
                close(tcp_s);
                exit(1);
            }
            recvd += len;
            string str(buf);
            cout << str;
        }
    }
}

void Client :: list_boards() {
    send_udp_string("LIS");
    string listing = receive_udp_string();
    cout << "Board listing:" << endl;
    cout << listing << endl;
}

bool Client :: shutdwn() {
    send_udp_string("SHT");

    string password;
    cout << "Enter admin password: ";
    cin >> password;
    send_udp_string(password);
    int response = receive_udp_int();
    if (response == 1) {
        close_sockets();
        return false;
    } else {
        cout << "Incorrect password" << endl;
        return true;
    }
}
