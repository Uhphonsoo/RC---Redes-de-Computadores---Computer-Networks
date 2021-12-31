// TODO
/**
 * time out in select
 * validate_program_input()
 * process_input()
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


int main(int argc, char *argv[]) {

    int n;
    int fd_TCP = 0, fd_UDP = 0, client_fd;
    struct addrinfo hints_TCP, *res_TCP, hints_UDP, *res_UDP;
    struct sockaddr_in clientaddr;
    socklen_t clientlen;
    fd_set current_sockets, ready_sockets;
    // socklen_t addrlen_UDP, addrlen_TCP, addrlen; /* DEBUG */
    // struct sockaddr_in addr_UDP, addr_TCP, addr; /* DEBUG */

    fd_TCP = create_socket_stream();
    get_address_info_stream(&hints_TCP, &res_TCP, PORT);

    n = bind(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    validate_bind(n);

    if (listen(fd_TCP, 5) == -1) {
        perror("ERROR: listen\n");
        exit(EXIT_FAILURE);
    }

    fd_UDP = create_socket_datagram();
    get_address_info_datagram(&hints_UDP, &res_UDP, PORT);

    n = bind(fd_UDP, res_UDP->ai_addr, res_UDP->ai_addrlen);
    validate_bind(n);

    /* initialize current sets of file descriptors */
    FD_ZERO(&current_sockets);
    FD_SET(fd_TCP, &current_sockets);
    FD_SET(fd_UDP, &current_sockets);

    while (1) {

        // make a copy of the file descriptor set
        ready_sockets = current_sockets;

        /* Block server until timeout */
        select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL);
        // TODO: validate_select

        /* Check for requests */
        for (int i = 0; i < FD_SETSIZE; i++) {

            if (FD_ISSET(i, &ready_sockets)) {
                if (i == fd_TCP) {

                    /* Accept client and associate it with client_fd */
                    clientlen = sizeof(clientaddr);
                    client_fd = accept(fd_TCP, (struct sockaddr *) &clientaddr, &clientlen);
                    FD_SET(client_fd, &current_sockets);
                }
                else if (i == fd_UDP) {

                    receive_message_UDP(fd_UDP);

                    /* DEBUG */
                    printf(">>> UDP: message = %s|\n", message);

                    process_message();

                    FD_CLR(i, &current_sockets);
                    clear_string(message);
                }
                // if i == client_fd
                else {

                    receive_message_TCP(i);

                    /* DEBUG */
                    printf(">>> TCP: message = %s|\n", message);
                    
                    process_message();
                    
                    FD_CLR(i, &current_sockets);
                    clear_string(message);
                }
            }
        }
    }

    freeaddrinfo(res_UDP);
    close(fd_UDP);
    freeaddrinfo(res_TCP);
    close(fd_TCP);

    return EXIT_SUCCESS;
}


