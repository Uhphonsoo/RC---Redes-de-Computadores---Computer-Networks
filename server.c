// TODO
/**
 * time out in select
 * validate_program_input
**/

// ISSUES
/**
 * 
**/

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

int  fd_UDP, fd_TCP;
int  errcode;
int  logged_in;
int  has_active_group;
char message[MAX_SIZE];
char reply[MAX_REPLY_SIZE];
char message_buffer[MAX_SIZE];
char reply_buffer[MAX_REPLY_SIZE];
char DSIP[MAX_SIZE];
char DSport[MAX_SIZE];
char logged_in_UID[MAX_SIZE];
char logged_in_pass[MAX_SIZE];
char active_GID[MAX_SIZE];
char buffer_aux[1024];
socklen_t addrlen_UDP, addrlen_TCP, addrlen; /* DEBUG */
struct addrinfo hints_UDP, *res_UDP, hints_TCP, *res_TCP;
struct sockaddr_in addr_UDP, addr_TCP, addr; /* DEBUG */


int main(int argc, char *argv[]) {

    int n;
    int server_stream_fd = 0, server_datagram_fd = 0, client_fd;
    struct addrinfo hints_strm, *res_strm, hints_dgrm, *res_dgrm;
    struct sockaddr_in servstrmaddr, servdgrmaddr, clientaddr;
    int len;
    socklen_t clientlen;
    fd_set current_sockets_strm, ready_sockets_strm;
    fd_set current_sockets_dgrm, ready_sockets_dgrm;


    server_stream_fd = create_socket_stream();

    /* DEBUG */
    printf("server_stream_fd = %d\n", server_stream_fd);

    int enable = 1;
    if (setsockopt(server_stream_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt\n");
        exit(EXIT_FAILURE);
    }

    memset(&hints_strm, 0, sizeof(hints_strm)); 
    hints_strm.ai_family = AF_INET; // IPv4
    hints_strm.ai_socktype = SOCK_STREAM; // TCP socket
    hints_strm.ai_flags = AI_PASSIVE;

    /* DEBUG port */
    errcode = getaddrinfo(NULL, PORT, &hints_strm, &res_strm);
    validate_getaddrinfo(errcode);
    /* get_address_info_stream(&hints_strm, &res_strm, PORT); */

    n = bind(server_stream_fd, res_strm->ai_addr, res_strm->ai_addrlen);
    validate_bind(n);

    if (listen(server_stream_fd, 5) == -1) {
        perror("ERROR: listen\n");
        exit(EXIT_FAILURE);
    }



    server_datagram_fd = create_socket_datagram();

    /* DEBUG */
    printf("server_datagram_fd = %d\n", server_datagram_fd);

    /* int enable = 1;
    if (setsockopt(server_datagram_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
        perror("setsockopt\n");
        exit(EXIT_FAILURE);
    } */

    memset(&hints_dgrm, 0, sizeof(hints_dgrm));
    hints_dgrm.ai_family = AF_INET; 
    hints_dgrm.ai_socktype = SOCK_DGRAM; 
    hints_dgrm.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, PORT, &hints_dgrm, &res_dgrm);
    validate_getaddrinfo(errcode);
    /* get_address_info_datagram(&hints_dgrm, &res_dgrm, PORT_CHILD); */

    n = bind(server_datagram_fd, res_dgrm->ai_addr, res_dgrm->ai_addrlen);
    validate_bind(n);
    

    //while(1) {
        /* receive_message_UDP();
        process_message(); */

        /* DEBUG */
        /* printf(">>> ECHO2\n");
        addrlen = sizeof(addr);
        n = recvfrom(server_datagram_fd, buffer_aux, 1024, 0, (struct sockaddr*)&addr, &addrlen);
        validate_recvfrom(n);
        printf(">>> received %s\n", buffer_aux);
        n = sendto(server_datagram_fd, buffer_aux, n, 0, (struct sockaddr*)&addr, addrlen);
        validate_sendto(n);
        printf(">>> ECHO3\n"); */
    //}
    

    /* initialize current sets of file descriptors */
    FD_ZERO(&current_sockets_strm);
    FD_SET(server_stream_fd, &current_sockets_strm);
    FD_SET(server_datagram_fd, &current_sockets_strm);

    while (1) {
        /* DEBUG */
        printf(">>> entered\n");

        // make a copy of the file descriptor set
        ready_sockets_strm = current_sockets_strm;

        /* Block server until timeout */
        select(FD_SETSIZE, &ready_sockets_strm, NULL, NULL, NULL);
        // TODO: validate_select

        /* Check for requests */
        for (int i = 0; i < FD_SETSIZE; i++) {

            /* DEBUG */
            printf("i = %d\n", i);

            if (FD_ISSET(i, &ready_sockets_strm)) {
                if (i == server_stream_fd) {
                    /* DEBUG */
                    printf(">>> new connection stream\n");

                    /* Accept client and associate it with client_fd */
                    clientlen = sizeof(clientaddr);
                    client_fd = accept(server_stream_fd, (struct sockaddr *) &clientaddr, &clientlen);
                    FD_SET(client_fd, &current_sockets_strm);

                    /* receive_message_TCP();
                    process_message(); */

                    /* DEBUG */
                    /* addrlen = sizeof(addr);
                    n = read(client_fd, buffer_aux, 1024);
                    if (n == -1) exit(1);
                    write(1, "received: ", 10);
                    write(1, buffer_aux, n);
                    n = write(client_fd, buffer_aux, n);
                    if (n == -1) exit(1); */
                }
                else if (i == server_datagram_fd) {
                    /* DEBUG */
                    printf(">>> new connection stream\n");

                    /* printf(">>> ECHO2\n");
                    addrlen = sizeof(addr);
                    n = recvfrom(server_datagram_fd, buffer_aux, 1024, 0, (struct sockaddr*)&addr, &addrlen);
                    validate_recvfrom(n);
                    printf(">>> received %s\n", buffer_aux);
                    n = sendto(server_datagram_fd, buffer_aux, n, 0, (struct sockaddr*)&addr, addrlen);
                    validate_sendto(n);
                    printf(">>> ECHO3\n"); */
                    
                    FD_CLR(i, &current_sockets_strm);
                }
                // if i == clientfd
                else {
                    /* DEBUG */
                    printf(">>> connection ??? \n");

                    /* DEBUG */
                    addrlen = sizeof(addr);
                    n = read(i, buffer_aux, 1024);
                    validate_read(n);
                    printf(">>> message = %s\n", buffer_aux);
                    n = write(i, buffer_aux, n);
                    validate_write(n);

                    /* receive_message_TCP();
                    process_message(); */
                    
                    FD_CLR(i, &current_sockets_strm);
                    /* DEBUG */
                    printf("CLEARED\n");
                }
            }
        }
    }

    freeaddrinfo(res_dgrm);
    close(server_datagram_fd);
    freeaddrinfo(res_strm);
    close(server_stream_fd);

    return EXIT_SUCCESS;
}


