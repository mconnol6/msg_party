// main.cpp - client
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling

#include "client.h"
using namespace std;

int main(int argc, char** argv) {
    
    if (argc != 3) {
        cerr << "usage: myfrm serverName portNumber" << endl;
        exit(1);
    }
    
    Client c;
    
    if (!c.connect_to_server(argv[1], atoi(argv[2]))) {
        return 0;
    }

    c.send_input();
    return 0;
}
