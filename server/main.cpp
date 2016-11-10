#include "server.h"
using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "usage: myfrmd portNumber adminPassword" << endl;
        exit(1);
    }

    Server s(atoi(argv[1]), argv[2]);
    s.accept_connections();
    s.receive_input();
}
