// client.cpp
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling

#include "client.h"
#include "string.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
using namespace std;

Client :: Client() {
}

bool Client :: connect_to_server(char* hostname, int port) {
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
    tcp_sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&tcp_sin.sin_addr, hp->h_length);
    tcp_sin.sin_port = htons(port);


    // Open UDP Socket
    if ((udp_s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "myfrm: udp socket" << endl;
        exit(1);
    }
    
    addr_len = sizeof(udp_sin);

    // Open TCP Socket
    if ((tcp_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "myfrm: tcp socket" << endl;
        exit(1);
    }

    // Connect to server
    if (connect(tcp_s, (struct sockaddr *)&tcp_sin, sizeof(tcp_sin)) < 0) {
        cerr << "myfrm: connect\n" << endl;
        close(tcp_s);
        exit(1);
    }
    
    bool success = signin_user();

    if (!success) {
        close_sockets();
        return false;
    }

    return true;
}

bool Client :: send_udp_string(string str) {
    if (sendto(udp_s, str.c_str(), str.length(), 0, (struct sockaddr*) &udp_sin, sizeof(struct sockaddr_in)) == -1) {
        perror("Client send error\n");
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

void Client :: send_tcp_file(string filename) {
    char buf[4096];
    int len;
    FILE *fp = fopen(filename.c_str(), "r");
    
    while ((len = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
        if (send(tcp_s, buf, len, 0) == -1) {
            cerr << "TCP Send error!" << endl;
            exit(1);
        }
        bzero(buf, sizeof(buf));
    }
    fclose(fp);   
}

void Client :: receive_tcp_file(string attachment, int filesize) {
    FILE *fp = fopen(attachment.c_str(), "w+");
    char buf[4096];
    int len;

    if (!fp) {
        return;
    }

    int total_read = 0;

    while(total_read < filesize) {
        bzero(buf, sizeof(buf));
        if ((len = recv(tcp_s, buf, sizeof(buf), 0)) == -1) {
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

void Client :: ack() {
    send_udp_int(1);
}

string Client :: receive_udp_string() {
    char buf[4096];
    bzero(buf, sizeof(buf));

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

//returns 1 if signin successful and 0 otherwise
bool Client :: signin_user() {
    send_udp_int(1);
    int ack = receive_udp_int();
    if (ack != 1) {
        cout << "Ack error" << endl;
        return false;
    }

    string username, password;
    cout << "Enter username: ";
    cin >> username;
    send_udp_string(username);

    ack = receive_udp_int();
    if (ack != 1) {
        cout << "Ack error" << endl;
        return false;
    }

    cout << "Enter password: ";
    cin >> password;
    send_udp_string(password);

    ack = receive_udp_int();

    if (ack == 1) {
        return true;
    } else {
        cout << "Incorrect password" << endl;
        return false;
    }
}

void Client :: send_input() {
    string command;

    bool cont = true;
    while (cont) {        
        print_commands();
        cin >> command;
        if (command == "CRT") {
            create_board();
        } else if (command == "MSG") {
            post_message();
        } else if (command == "DLT") {
            delete_message();
        } else if (command == "EDT") {
            edit_message();
        } else if (command == "LIS") {
            list_boards();
        } else if (command == "RDB") {
            read_board();
        } else if (command == "APN") {
            append_file();
        } else if (command == "DWN") {
            download_file();
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

void Client :: create_board() {
    string board;
    send_udp_string("CRT");
    cout << "Enter name of board to create: ";
    cin >> board;
    send_udp_string(board);
    int success = receive_udp_int();
    if (success) {
        cout << "Board created successfully." << endl;
    } else if (success == -1) {
        cout << "The board was not created successfully." << endl;
    } else {
        cout << "The board already exists." << endl;
    }
}

void Client :: post_message() {
    string board;
    send_udp_string("MSG");
    
    cout << "Enter name of board to post to: ";
    cin >> board;
    cin.ignore();
    send_udp_string(board);
    
    char buf[1024];
    cout << "Enter message: ";
    cin.getline(buf, sizeof(buf));
    string msg(buf);
    send_udp_string(msg);

    int success = receive_udp_int();
    if (success == 1) {
        cout << "Message successfully added to board" << endl;
    } else {
        cout << "Error: board does not exist!" << endl;
    }
}

void Client :: delete_message() {
    string board;
    send_udp_string("DLT");

    cout << "Enter name of board to delete from: ";
    cin >> board;
    send_udp_string(board);

    int msg;
    cout << "Enter index of message to delete: ";
    cin >> msg;
    send_udp_int(msg);

    int status = receive_udp_int();
    if (status == -2) {
        cout << "Error: Invalid board" << endl;
    } else if (status == -1) {
        cout << "Error: Invalid index" << endl;
    } else if (status == 0) {
        cout << "Error: Can't delete another user's message" << endl;
    } else {
        cout << "Message successfully deleted" << endl;
    }
}

void Client :: edit_message() {
    string board;
    send_udp_string("EDT");

    cout << "Enter name of board to edit: ";
    cin >> board;
    send_udp_string(board);

    int msg;
    cout << "Enter index of message to edit: ";
    cin >> msg;
    cin.ignore();
    send_udp_int(msg);

    char buf[1024];
    cout << "Enter new message: ";
    cin.getline(buf, sizeof(buf));
    string message(buf);
    send_udp_string(message);

    int status = receive_udp_int();
    if (status == -2) {
        cout << "Error: Invalid board" << endl;
    } else if (status == -1) {
        cout << "Error: Invalid index" << endl;
    } else if (status == 0) {
        cout << "Error: Can't edit another user's message" << endl;
    } else {
        cout << "Message successfully edited" << endl;
    }
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

void Client :: append_file() {
    int file_size;
    struct stat st;

    //get user input
    string board, filename;
    cout << "Enter name of board: ";
    cin >> board;
    cout << "Enter file name: ";
    cin >> filename;

    send_udp_string("APN");
    send_udp_string(board);
    send_udp_string(filename);

    int success = receive_udp_int();
    if (success == 0) {
        cout << "Error: board does not exist." << endl;
        return;
    }

    if (success == -1) {
        cout << "Error: file has already been attached to the board." << endl;
        return;
    }

    if (success) {
        //make sure that file exists
        if (access(filename.c_str(), F_OK) == -1) {
            cout << "The file does not exist." << endl;
            send_udp_int(-1);
            return;
        }

        //get file size
        stat(filename.c_str(), &st);
        file_size = st.st_size;
        send_udp_int(file_size);

        //send file
        send_tcp_file(filename);

        cout << "Success!" << endl;
    }
}

void Client :: download_file() {
    send_udp_string("DWN");
    string board, filename;

    //get user input
    cout << "Enter name of board: ";
    cin >> board;
    cout << "Enter name of file: ";
    cin >> filename;

    send_udp_string(board);
    send_udp_string(filename);

    int file_size = receive_udp_int();

    if (file_size == -1) {
        cout << "Error: board does not exist." << endl;
        return;
    }

    if (file_size == -2) {
        cout << "Error: attachment does not exist." << endl;
        return;
    }

    receive_tcp_file(filename, file_size);

    cout << "Success!" << endl;
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
