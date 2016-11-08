// client.c
// John Rocha, Mary Connolly, Paul Dowling
// jrocha2, mconnol6, pdowling

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <fcntl.h>

int udp_s = 0;
int tcp_s = 0;

void print_commands();

int main(int argc, char** argv) {
    
    // Check for valid input arguments
    if (argc != 3) {
        fprintf(stderr, "usage: ./myfrm serverName portNumber\n");
        exit(1);
    }

    struct hostent *hp;
    struct sockaddr_in sin;
    char buf[100];

    // Translate host name into IP address
    hp = gethostbyname(argv[1]);
    if (!hp) {
        fprintf(stderr, "myfrm: unknown host: %s\n", argv[1]);
        exit(1);
    }

    // Build socket-in address
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(atoi(argv[2]));

    // Open UDP socket
    if ((udp_s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("myfrm: udp socket\n");
        exit(1);
    }

    // Open TCP socket
    if ((tcp_s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("myfrm: tcp socket\n");
        exit(1);
    }

    // Connect to server
    if (connect(tcp_s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("myfrm: connect\n");
        close(tcp_s);
        exit(1);
    }

    print_commands();
    while (fgets(buf, sizeof(buf), stdin)) {        
        if (!strncmp(buf, "CRT", 3)) {
        } else if (!strncmp(buf, "MSG", 3)) {
        } else if (!strncmp(buf, "DLT", 3)) {
        } else if (!strncmp(buf, "EDT", 3)) {
        } else if (!strncmp(buf, "LIS", 3)) {
        } else if (!strncmp(buf, "RDB", 3)) {
        } else if (!strncmp(buf, "APN", 3)) {
        } else if (!strncmp(buf, "DWN", 3)) {
        } else if (!strncmp(buf, "DST", 3)) {
        } else if (!strncmp(buf, "XIT", 3)) {
        } else if (!strncmp(buf, "SHT", 3)) {
        } else {
            printf("\nInvalid Operation\n");
        }
        
        buf[99] = '\0';
        int len = strlen(buf) + 1;
        if (sendto(udp_s, buf, len, 0, (struct sockaddr*) &sin, sizeof(struct sockaddr)) == -1) {
            perror("\nClient send error");
            exit(1);
        }
        print_commands();
    }

    close(udp_s);
    close(tcp_s);
    return 0;
}


void print_commands() {
    printf("\nEnter one of the following commands:\n");
    printf("CRT: Create Board \nMSG: Leave Message \nDLT: Delete Message\n");
    printf("EDT: Edit Message \nLIS: List Boards \nRDB: Read Board\n");
    printf("APN: Append File \nDWN: Download File \nDST: Destroy Board\n");
    printf("XIT: Exit Client \nSHT: Shutdown Server\n");
    printf("Your command: ");
}
