// main.cpp
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling
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
        bool accepted = s.accept_connections();

        //check if signin was successful
        if (accepted) {
            cont = s.receive_input();
        }
    }
}
