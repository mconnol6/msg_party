#include "server.h"
using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "usage: myfrmd portNumber" << endl;
        exit(1);
    }

    Server s;
    s.initialize_server(atoi(argv[1]));
    s.accept_connections();
    s.receive_input();
}
