// client.h
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <iostream>
using namespace std;

class Client {
    public:
        Client();
        void connect_to_server(char*, int);
        void print_commands();
        void send_input();

    private:
        int udp_s;
        int tcp_s;
        struct sockaddr_in sin;
};