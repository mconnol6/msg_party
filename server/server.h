// server.h
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
using namespace std;

class Server {
    public:
        Server();
        void initialize_server(int);
        void accept_connections();
        void receive_input();

    private:
        int udp_s;
        int tcp_s;
        int new_tcp_s;
        int addr_len;
        struct sockaddr_in udp_sin;
        struct sockaddr_in tcp_sin;
        struct sockaddr_in client_addr;
};
