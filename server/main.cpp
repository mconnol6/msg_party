#include "server.h"
using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "usage: myfrmd portNumber adminPassword" << endl;
        exit(1);
    }

    bool cont = true;
    Server s(atoi(argv[1]), argv[2]);
    while (cont) {
        s.accept_connections();
        cont = s.receive_input();
    }
}
