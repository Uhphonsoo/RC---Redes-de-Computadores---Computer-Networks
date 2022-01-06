// TODO
/**
 * time out in select
 * check allocated sizes for all strings
 * frees for all mallocs
 * validate_post_message
**/

// ISSUES
/**
 * reply in ulist_command is sometimes read incorrectly (???)
 * not sure if get_client_ip_and_port is working as intended
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
/* socklen_t addrlen_UDP;
struct sockaddr_in addr_UDP; */
// int  logged_in;
// int  has_active_group;
// char message[MAX_SIZE];
// char Reply[MAX_REPLY_SIZE];
// char message_buffer[MAX_SIZE];
// char reply_buffer[MAX_REPLY_SIZE];
// char DSIP[MAX_SIZE];
// char logged_in_UID[MAX_SIZE];
// char logged_in_pass[MAX_SIZE];
// char active_GID[MAX_SIZE];
// char buffer_aux[1024];


int main(int argc, char *argv[]) {

    int n;
    int fd_TCP = 0, fd_UDP = 0, client_fd;
    struct addrinfo hints_TCP, *res_TCP, hints_UDP, *res_UDP, *res_client, hints_client;
    struct sockaddr_in clientaddr;
    socklen_t clientlen;
    fd_set current_sockets, ready_sockets;
    char DSport[MAX_SIZE];
    /* char message[MAX_SIZE]; */
    char *message;
    struct sockaddr_in addr;
    // socklen_t addrlen_UDP, addrlen_TCP, addrlen; /* DEBUG */
    // struct sockaddr_in addr_UDP, addr_TCP, addr; /* DEBUG */

    validate_program_input(argc, argv, DSport);

    Group_list = malloc(sizeof(*Group_list));
    Number_of_groups = get_groups(Group_list);

    // initialize group list
    initialize_group_list(Group_list);

    /* DEBUG */
    /* strcpy(Group_list->group_name[0], "teste");
    printf(">>> Group_list->group_name[0] = %s|\n", Group_list->group_name[0]);
    printf(">>> Number_of_groups = %d\n", Number_of_groups); */

    fd_TCP = create_socket_stream();
    get_address_info_stream(&hints_TCP, &res_TCP, DSport);

    n = bind(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    validate_bind(n);

    if (listen(fd_TCP, 5) == -1) {
        perror("ERROR: listen\n");
        exit(EXIT_FAILURE);
    }

    fd_UDP = create_socket_datagram();
    get_address_info_datagram(&hints_UDP, &res_UDP, DSport);

    n = bind(fd_UDP, res_UDP->ai_addr, res_UDP->ai_addrlen);
    validate_bind(n);

    /* initialize current sets of file descriptors */
    FD_ZERO(&current_sockets);
    FD_SET(fd_TCP, &current_sockets);
    FD_SET(fd_UDP, &current_sockets);

    while (1) {

        /* FD_ZERO(&current_sockets); */
        /* FD_SET(fd_TCP, &current_sockets); */
        FD_SET(fd_UDP, &current_sockets);

        /* DEBUG */
        /* printf(">>> ECHO while\n"); */

        // make a copy of the file descriptor set
        ready_sockets = current_sockets;

        /* Block server until timeout */
        n = select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL);
        validate_select(n);

        /* Check for requests */
        for (int i = 0; i < FD_SETSIZE; i++) {

            if (FD_ISSET(i, &ready_sockets)) {

                /* DEBUG */
                /* printf(">>> ECHO 1\n"); */

                if (i == fd_TCP) {

                    /* DEBUG */
                    /* printf(">>> ECHO 2\n"); */

                    /* Accept client and associate it with client_fd */
                    clientlen = sizeof(clientaddr);
                    client_fd = accept(fd_TCP, (struct sockaddr *) &clientaddr, &clientlen);
                    FD_SET(client_fd, &current_sockets);
                }
                else if (i == fd_UDP) {

                    /* DEBUG */
                    printf(">>> ECHO 3\n");

                    message = (char *)malloc(MAX_SIZE);
                    message[0] = '\0';

                    receive_message_UDP(fd_UDP, message, &addr);

                    /* DEBUG */
                    printf(">>> UDP: message = %s|\n", message);

                    process_message(message, fd_UDP, &addr);

                    FD_CLR(i, &current_sockets);
                    free(message);
                }
                // if i == client_fd
                else {
                    
                    char keyword[10];
                    /* DEBUG */
                    printf(">>> ECHO 4\n");

                    message = (char *)malloc(MAX_SIZE);

                    /* DEBUG */
                    /* n = read(i, message, 10); */

                    // receive_message_TCP(i, message);

                    /* DEBUG */
                    /* printf(">>> TCP: message = %s|\n", message); */
                    
                    // process_message(message, i, &addr);

                    /* receive_keyword_TCP(keyword, i); */
                    receive_n_chars_TCP(3, keyword, i);

                    /* DEBUG */
                    printf(">>> >>> keyword = %s|\n", keyword);
                    
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


