// server.h
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_set>
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
        bool accept_connections();
        bool receive_input();
        string receive_udp_string();
        int receive_udp_int();
        void send_udp_string(string);
        void send_udp_int(int);
        void ack();
        bool signin_user();
        void send_tcp_file(string);
        void receive_tcp_file(string, int);
        void execute_command(string);
        void create_board();
        void post_message();
        void read_board();
        void list_boards();
        void append_file();
        bool shutdwn();
        void addMessage(string, string, string, bool);

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
        map<string, Board> boards;
        map<string, string> users;
        unordered_set<string> filenames;
};
