// server.cpp
#include "server.h"
using namespace std;

Server :: Server(int port, char* password) {
    initialize_server(port);
    string admin(password);
    admin_password = admin;
}

void Server :: initialize_server(int port) {
    
    //set up udp
    //build address data structure
    bzero((char *)&client_addr, sizeof(struct sockaddr_in));
    bzero((char *)&udp_sin, sizeof(struct sockaddr_in));
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
    if ((tcp_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

bool Server :: accept_connections() {
    int len = sizeof(tcp_sin);
    
    if ((new_tcp_s = accept(tcp_s, (struct sockaddr *)&tcp_sin, (socklen_t *)&len)) < 0) {
        perror("myfrmd: accept error");
        exit(1);
    }

    bool success = signin_user();
    
    //if signin was not successful, return false
    if (!success) {
        return false;
    }

    cout << "Client connected" << endl;

    return true;
}

bool Server :: receive_input() {
    while(1) {
        string str = receive_udp_string();

        if (str == "XIT") {
            return true;
        } else if (str == "SHT") {
            bool success = shutdwn();
            if (success == true) {
                return false;
            }
        } else {
            execute_command(str);
        }
    }
}

string Server :: receive_udp_string() {
    int ibytes;
    char buf[4096];
    bzero((char *)&buf, sizeof(buf));

    if ((ibytes = recvfrom(udp_s, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0) {
        cerr << "Receive error" << endl;
        exit(1);
    }

    string str(buf);
    return str;
}

int Server :: receive_udp_int() {
    int i;

    if ((recvfrom(udp_s, &i, sizeof(int), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0) {
        cerr << "Receive error" << endl;
        exit(1);
    }
    i = ntohl(i);

    return i; 
}

void Server :: send_udp_int(int i) {
    i = htonl(i);
    if (sendto(udp_s, &i, sizeof(int), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Server send error");
        exit(1);
    }
}

void Server :: ack() {
    send_udp_int(1);
}

//returns true if signin is successful and false otherwise
bool Server :: signin_user() {
    receive_udp_int();
    ack();
    string username, password;
    username = receive_udp_string();
    ack();
    password = receive_udp_string();

    //if user is not in map of users, add their username/password
    if (users.find(username) == users.end()) {
        users[username] = password;
        send_udp_int(1);
        return true;
    } else {
        //otherwise check if password matches the one in the map
        if (users[username] == password) {
            send_udp_int(1);
            return true;
        } else {
            send_udp_int(0);
            return false;
        }
    }
}

void Server :: execute_command(string command) {
    if (command == "CRT") {
    } else if (command == "MSG") {
    } else if (command == "DLT") {
    } else if (command == "EDT") {
    } else if (command == "LIS") {
    } else if (command == "RDB") {
    } else if (command == "APN") {
    } else if (command == "DWN") {
    }
}

//returns whether or not the server has been cleaned up/shutdown
bool Server :: shutdwn() {
    string password = receive_udp_string();
    if (password == admin_password) {
        send_udp_int(1);
        return true;
    } else {
        send_udp_int(0);
        return false;
    }
}
