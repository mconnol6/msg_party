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
        bool connect_to_server(char*, int);
        bool send_udp_string(string);
        void send_udp_int(int);
        void ack();
        string receive_udp_string();
        int receive_udp_int();
        void send_tcp_file(string);
        void receive_tcp_file(string, int);
        bool signin_user();
        void print_commands();
        void send_input();
        void close_sockets();
        void create_board();
        void list_boards();
        void read_board();
        void append_file();
        void download_file();
        bool shutdwn();

    private:
        int udp_s;
        int tcp_s;
        struct sockaddr_in udp_sin;
        struct sockaddr_in tcp_sin;
        int addr_len;
};
