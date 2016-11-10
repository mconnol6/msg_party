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
#include <vector>
#include <map>
using namespace std;

typedef struct Message {
    string msg;
    string user;
    bool is_appended;
} Message;

typedef struct Board {
    vector<Message> msgs;
    string creator;
    string filename;
} Board;

class Server {
    public:
        Server(int, char*);
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
        string admin_password;
        string current_user;
        vector<Board> boards;
        map<string, string> users;
};