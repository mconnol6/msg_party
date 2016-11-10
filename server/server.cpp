// server.cpp
#include "server.h"
using namespace std;


Server :: Server() {
}

void Server :: initialize_server(int port) {
    
    //set up udp
    //build address data structure
    bzero((char *)&udp_sin, sizeof(udp_sin));
    udp_sin.sin_family = AF_INET;
    udp_sin.sin_addr.s_addr = INADDR_ANY;
    udp_sin.sin_port = htons(port);

    //create socket
    if ((udp_s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "myfrmd: socket" << endl;
        exit(1);
    }

    //bind socket to address
    if ((bind(udp_s, (struct sockaddr *)&udp_sin, sizeof(udp_sin))) < 0) {
        cerr << "myfrmd: bind" << endl;
        exit(1);
    }

    addr_len = sizeof(client_addr);

    //set up tcp
    int opt=1;

    //build address data structure
    bzero((char *)&tcp_sin, sizeof(tcp_sin));
    tcp_sin.sin_family = AF_INET;
    tcp_sin.sin_addr.s_addr = INADDR_ANY;
    tcp_sin.sin_port = htons(port);

    //create socket
    if ((tcp_s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "myfrmd: socket error" << endl;
        exit(1);
    }

    //set socket option
    if ((setsockopt(tcp_s, SOL_SOCKET, SO_REUSEADDR, (char *)& opt, sizeof(int))) < 0) {
        cerr << "myfrmd: setsockopt error" << endl;
        exit(1);
    }

    //bind socket to address
    if((bind(tcp_s, (struct sockaddr *)&tcp_sin, sizeof(tcp_sin))) < 0) {
        cerr << "myfrmd: bind error" << endl;
        exit(1);
    }

    // listen to socket
    if ((listen(tcp_s, 5)) < 0) {
        cerr << "myfrmd: listen error" << endl;
        exit(1);
    }
}

void Server :: accept_connections() {
    int len;

    if ((new_tcp_s = accept(tcp_s, (struct sockaddr *)&tcp_sin, (socklen_t *)&len)) < 0) {
        cerr << "myfrmd: accept error" << endl;
        exit(1);
    }
    cout << "Client connected" << endl;
}

void Server :: receive_input() {
    int ibytes;
    char buf[4096];
    
    while(1) {
        bzero((char *)&buf, sizeof(buf));

        if ((ibytes = recvfrom(udp_s, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0) {
            cerr << "Receive error" << endl;
            exit(1);
        }

        string str(buf);
        cout << str << endl;
    }
}
