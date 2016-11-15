// server.cpp
#include "server.h"
#include <vector>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
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

void Server :: send_udp_string(string str) {
    if (sendto(udp_s, str.c_str(), str.length(), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in)) == -1) {
        cerr << "Server send error" << endl;
        exit(1);
    }
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
        current_user = username;
        send_udp_int(1);
        return true;
    } else {
        //otherwise check if password matches the one in the map
        if (users[username] == password) {
            send_udp_int(1);
            current_user = username;
            return true;
        } else {
            send_udp_int(0);
            return false;
        }
    }
}

void Server :: send_tcp_file(string filename) {
    char buf[4096];
    int len;
    FILE *fp = fopen(filename.c_str(), "r");
    
    while ((len = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
        if (send(new_tcp_s, buf, len, 0) == -1) {
            cerr << "TCP Send error!" << endl;
            exit(1);
        }
        bzero(buf, sizeof(buf));
    }
    fclose(fp);   
}

void Server :: receive_tcp_file(string attachment, int filesize) {
    FILE *fp = fopen(attachment.c_str(), "w+");
    char buf[4096];
    int len;

    if (!fp) {
        return;
    }

    int total_read = 0;

    while(total_read < filesize) {
        bzero(buf, sizeof(buf));
        if ((len = recv(new_tcp_s, buf, sizeof(buf), 0)) == -1) {
            perror("\nReceive error");
            close(tcp_s);
            exit(1);
        }

        total_read += len;
        
        //add to file
        fwrite(buf, sizeof(char), len, fp);
    }

    fclose(fp);
}

void Server :: execute_command(string command) {
    if (command == "CRT") {
        create_board();
    } else if (command == "MSG") {
        post_message();
    } else if (command == "DLT") {
    } else if (command == "EDT") {
    } else if (command == "LIS") {
        list_boards();
    } else if (command == "RDB") {
        read_board();
    } else if (command == "APN") {
        append_file();
    } else if (command == "DWN") {
    }
}

void Server :: create_board() {
    string board_name = receive_udp_string();

    //if filename aleady exists
    if (filenames.find(board_name) != filenames.end()) {
        send_udp_int(0);
        return;
    }

    //create board object
    Board b({ vector<Message>(), current_user, board_name });

    boards[board_name] = b;
    
    //create file for board
    FILE *f = fopen(board_name.c_str(), "w+");

    if (f) {
        filenames.insert(board_name);

        //add user's name to the top of the board
        fwrite((current_user + "\n").c_str(), sizeof(char), current_user.length() + 1, f);

        fclose(f);
        
        //send confirmation to user
        send_udp_int(1);
    } else {
        send_udp_int(-1);
    }

}

void Server :: post_message() {
    string board, msg;
    board = receive_udp_string();
    msg = receive_udp_string();

    if (filenames.find(board) != filenames.end()) {
        addMessage(board, msg, current_user, false);
        send_udp_int(1);
    } else {
        send_udp_int(-1);
    }
}

// sends board contents to client
void Server :: read_board() {
    int filesize;
    struct stat st;
    
    string filename = receive_udp_string();
    if (access(filename.c_str(), F_OK) != -1) {
        stat(filename.c_str(), &st);
        send_udp_int(st.st_size);
    } else {
        send_udp_int(-1);
        return;
    }
    send_tcp_file(filename);
}

// sends board listing to client
void Server :: list_boards() {
    string listing;
    if (boards.size() == 0) {
        listing = "No Boards Currently Created";
    } else {
        for (auto it = boards.begin(); it != boards.end(); it++) {
            listing += it->second.filename;
        }
    }
    send_udp_string(listing);
}

void Server :: append_file() {
    string board = receive_udp_string();
    string filename = receive_udp_string();

    //if board does not exist
    if (filenames.find(board) == filenames.end()) {
        send_udp_int(0);    
        return;
    } 

    string attachment = board + "-" + filename;

    //if attachment file name already exists
    if (filenames.find(attachment) != filenames.end()) {
        send_udp_int(-1);
        return;
    }
    
    send_udp_int(1);

    int filesize = receive_udp_int();

    //check if negative; if so then the file does not exist so return
    if (filesize < 0) {
        return;
    }

    receive_tcp_file(attachment, filesize);

    //add new message to board
    addMessage(board, "Appended " + attachment + " to the board.", current_user, true);
}

//returns whether or not the server has been cleaned up/shutdown
bool Server :: shutdwn() {
    string password = receive_udp_string();
    if (password == admin_password) {
        send_udp_int(1);

        //delete files
        for (auto it=filenames.begin(); it!=filenames.end(); it++) {
            unlink((*it).c_str());
        }
        return true;
    } else {
        send_udp_int(0);
        return false;
    }
}

void Server :: addMessage(string board, string msg, string user, bool is_appended) {
    Message m({ msg, user, is_appended });
    boards[board].msgs.push_back(m); 

    stringstream ss;
    ss << setw(5);

    if (!is_appended) {
        ss << boards[board].msgs.size()-1;
    } else {
        ss << "";
    }
    ss  << setw(10)
        << user
        << ": "
        << msg
        << endl;


    string add_msg = ss.str();

    FILE *fp = fopen(board.c_str(), "a");
    fwrite(add_msg.c_str(), sizeof(char), add_msg.length(), fp);

    fclose(fp);
}
