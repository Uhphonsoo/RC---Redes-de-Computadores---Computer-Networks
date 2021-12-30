#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include "functions.h"
#include "server_functions.h"
#include "constants.h"

extern int  errcode;
extern int fd, newfd, afd;
extern socklen_t addrlen;
extern struct addrinfo hints, *res;
extern struct sockaddr_in addr;

void  create_socket() {

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
    if(fd == -1) {
        perror("ERROR: create_UDP_socket: can't open socket.\n");
        exit(EXIT_FAILURE);
    }
}


void  get_address_info() {

    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_DGRAM;   // UDP socket

    errcode = getaddrinfo(NULL, PORT, &hints, &res);
    if(errcode != 0) {
        perror("ERROR: get_address_info_UDP\n");
        exit(EXIT_FAILURE);
    }
}