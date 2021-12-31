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

extern char message[MAX_SIZE];
extern char reply[MAX_REPLY_SIZE];

int create_socket_datagram() {

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("ERROR: socket: create_socket_datagram\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}


void get_address_info_datagram(struct addrinfo *hints, struct addrinfo **res, char *port) {

    memset(&(*hints), 0, sizeof((*hints))); 
    (*hints).ai_family = AF_INET;        
    (*hints).ai_socktype = SOCK_DGRAM;   
    (*hints).ai_flags = AI_PASSIVE;

    int errcode = getaddrinfo(NULL, port, &(*hints), &(*res));
    if(errcode != 0) {
        perror("ERROR: getaddrinfo: get_address_info_datagram\n");
        exit(EXIT_FAILURE);
    }
}


int create_socket_stream() {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("ERROR: socket: create_socket_stream\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}


void get_address_info_stream(struct addrinfo *hints, struct addrinfo **res, char *port) {

    memset(&(*hints), 0, sizeof((*hints))); 
    (*hints).ai_family = AF_INET;        
    (*hints).ai_socktype = SOCK_STREAM;  
    (*hints).ai_flags = AI_PASSIVE;

    int errcode = getaddrinfo(NULL, port, &(*hints), &(*res));
    if(errcode != 0) {
        perror("ERROR: getaddrinfo: get_address_info_stream\n");
        exit(EXIT_FAILURE);
    }
}


/* void create_UDP_server_socket() {

    fd_UDP = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
    if(fd_UDP == -1) {
        perror("ERROR: socket: create_UDP_socket\n");
        exit(EXIT_FAILURE);
    }
} */


/* void get_address_info_server_UDP() {

    memset(&hints_UDP, 0, sizeof(hints_UDP)); 
    hints_UDP.ai_family = AF_INET;        // IPv4
    hints_UDP.ai_socktype = SOCK_DGRAM;   // UDP socket

    errcode = getaddrinfo(NULL, PORT, &hints_UDP, &res_UDP);
    if(errcode != 0) {
        perror("ERROR: get_address_info_UDP\n");
        exit(EXIT_FAILURE);
    }
} */


/* void create_TCP_server_socket() {

    fd_TCP = socket(AF_INET, SOCK_STREAM, 0); // TCP Socket
    if(fd_TCP == -1) {
        perror("ERROR: create_TCP_socket: can't open socket.\n");
        exit(EXIT_FAILURE);
    }
} */


/* void get_address_info_server_TCP() {

    memset(&hints_TCP, 0, sizeof(hints_TCP)); 
    hints_TCP.ai_family = AF_INET;        // IPv4
    hints_TCP.ai_socktype = SOCK_STREAM;  // TCP socket

    errcode = getaddrinfo(NULL, PORT, &hints_TCP, &res_TCP);
    if(errcode != 0) {
        perror("ERROR: get_address_info_TCP\n");
        exit(EXIT_FAILURE);
    }
} */


void receive_message_UDP(int fd) {

    int n;
    struct sockaddr_in addr;
    socklen_t addrlen ;
    
    addrlen = sizeof(addr);
    n = recvfrom(fd, message, MAX_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
    validate_recvfrom(n);
}


void receive_message_TCP(int fd) {

    int n;
    struct sockaddr_in addr;
    socklen_t addrlen ;

    addrlen = sizeof(addr);
    n = read(fd, message, MAX_SIZE);
    validate_read(n);
}



void process_message() {

}

