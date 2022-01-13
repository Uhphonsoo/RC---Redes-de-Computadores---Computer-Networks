// TODO
/**
 * ERR
 * check allocated sizes for all strings
 * frees for all mallocs
 * validate_post_message
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <ctype.h>
#include "functions.h"
#include "DS_functions.h"
#include "constants.h"

int verbose_mode;
int Number_of_groups;
GROUPLIST *Group_list;


int main(int argc, char *argv[]) {

    int ret;
    int fd_TCP = 0, fd_UDP = 0, client_fd;
    struct addrinfo hints_TCP, *res_TCP, hints_UDP, *res_UDP, *res_client, hints_client;
    struct sockaddr_in clientaddr;
    socklen_t clientlen;
    fd_set current_sockets, ready_sockets;
    char DSport[MAX_SIZE];
    char *message;
    char keyword[10];
    struct sockaddr_in addr;
    struct timeval timeout = {5, 0};

    validate_program_input(argc, argv, DSport);

    Group_list = malloc(sizeof(*Group_list));

    // initialize group list
    initialize_group_list(Group_list);
    Number_of_groups = get_groups(Group_list);

    // create TCP socket
    fd_TCP = create_socket_TCP();
    get_address_info_TCP(&hints_TCP, &res_TCP, DSport);

    ret = bind(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    validate_bind(ret);

    ret = listen(fd_TCP, 5);
    validate_listen(ret);

    // create UDP socket
    fd_UDP = create_socket_UDP();
    get_address_info_UDP(&hints_UDP, &res_UDP, DSport);

    ret = bind(fd_UDP, res_UDP->ai_addr, res_UDP->ai_addrlen);
    validate_bind(ret);

    /* initialize sets of file descriptors */
    FD_ZERO(&current_sockets);
    FD_SET(fd_TCP, &current_sockets);
    FD_SET(fd_UDP, &current_sockets);

    while (1) {

        // add UDP socket to file descriptor set
        FD_SET(fd_UDP, &current_sockets);

        // make a copy of the file descriptor set
        ready_sockets = current_sockets;

        ret = select(FD_SETSIZE, &ready_sockets, NULL, NULL, &timeout);
        validate_select(ret);

        /* Check for requests */
        for (int i = 0; i < FD_SETSIZE; i++) {

            if (FD_ISSET(i, &ready_sockets)) {

                if (i == fd_TCP) {

                    /* accept client and associate it with client_fd */
                    clientlen = sizeof(clientaddr);
                    client_fd = accept(fd_TCP, (struct sockaddr *) &clientaddr, &clientlen);
                    FD_SET(client_fd, &current_sockets);
                }
                else if (i == fd_UDP) {

                    message = (char *)malloc(MAX_SIZE);
                    message[0] = '\0';

                    receive_message_UDP(fd_UDP, message, &addr);
                    process_message(message, fd_UDP, &addr);

                    // remove UDP socket from file descriptor set
                    FD_CLR(i, &current_sockets);
                    free(message);
                }
                else { // if i == client_fd
                    
                    message = (char *)malloc(MAX_SIZE);

                    // receive keyword
                    receive_n_chars_TCP(3, keyword, i);
                    process_keyword(keyword, i, &addr);
                    
                    FD_CLR(i, &current_sockets);
                    free(message);
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


