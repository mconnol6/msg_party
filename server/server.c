/*Mary Connolly
 *netid: mconnol6
 */

#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<time.h>
#include<stdint.h>
#include<sys/time.h>

int udp_s; //udp socket
int new_tcp_s; //tcp socket
int addr_len;
struct sockaddr_in client_addr, udp_sin;

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: server [port number]\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    char buf[5000]; //buffer for udp message
    int obytes; //bytes sent in sendto() function
    int ibytes; //bytes received from client

    //set up udp
    //build address data structure
    bzero((char *)&udp_sin, sizeof(udp_sin));
    udp_sin.sin_family = AF_INET;
    udp_sin.sin_addr.s_addr = INADDR_ANY;
    udp_sin.sin_port = htons(port);

    //create socket
    if ((udp_s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("simplex-talk: socket\n");
        exit(1);
    }

    //bind socket to address
    if ((bind(udp_s, (struct sockaddr *)&udp_sin, sizeof(udp_sin))) < 0) {
        perror("simplex-talk: bind\n");
        exit(1);
    }

    addr_len = sizeof(client_addr);

    //set up tcp
    struct sockaddr_in tcp_sin;
    int tcp_s;
    int opt=1;
    int len;

    //build address data structure
    bzero((char *)&tcp_sin, sizeof(tcp_sin));
    tcp_sin.sin_family = AF_INET;
    tcp_sin.sin_addr.s_addr = INADDR_ANY;
    tcp_sin.sin_port = htons(port);

    //create socket
    if ((tcp_s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket\n");
        exit(1);
    }

    //set socket option
    if ((setsockopt(tcp_s, SOL_SOCKET, SO_REUSEADDR, (char *)& opt, sizeof(int))) < 0) {
        perror("simplex-talk: setsockopt\n");
        exit(1);
    }

    //bind socket to address
    if ((bind(tcp_s, (struct sockaddr *)&tcp_sin, sizeof(tcp_sin))) < 0) {
        perror("simplex-talk: bind\n");
        exit(1);
    }

    //listen to socket
    if ((listen(tcp_s, 5)) < 0) {
        perror("simplex-talk: listen\n");
        exit(1);
    }

    //accept an incoming connection
    while(1) {
        if ((new_tcp_s = accept(tcp_s, (struct sockaddr *)&tcp_sin, &len)) < 0) {
            perror ("simplex talk: accept\n");
            exit(1);
        }

        printf("\nClient connected\n");
        while (1) {
            bzero((char *)&buf, sizeof(buf));

            if ((ibytes = recvfrom(udp_s, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
                perror("Receive error!\n");
                exit(1);
            }
            printf("%s\n", buf);
        }

        printf("Client finished, close the connection.\n");
        close(new_tcp_s);
    }

    close(udp_s);

    return 0;
}
