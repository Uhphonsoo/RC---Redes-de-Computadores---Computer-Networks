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
socklen_t addrlen_UDP, addrlen_TCP;
struct addrinfo hints_UDP, *res_UDP, hints_TCP, *res_TCP;
struct sockaddr_in addr_UDP, addr_TCP;

int fd, newfd, afd;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;

int main(int argc, char *argv[]) {

    // TODO
    // validate_program_input(argc, argv);

    int n;
    fd_set rfds;
    enum {idle, busy} state;
    int maxfd, counter;
    socklen_t addrlen;
    struct sockaddr_in addr;
    char buffer[128];

    create_socket();
    get_address_info();

    /* DEBUG */
    printf(">>> server: ECHO1\n");

    state = idle;
    while (1) {
        /* DEBUG */
        printf(">>> server: ECHO2\n");

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        maxfd = fd;

        if (state == busy) {
            /* DEBUG */
            printf(">>> server: BUSY\n");

            FD_SET(afd, &rfds);
            maxfd = max(maxfd, afd);
        }

        /* DEBUG */
        printf(">>> server: ECHO3\n");

        /* !!! time out is done here */
        if ((counter = select(maxfd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval *) NULL)) < 0) {
            perror("ERROR: select\n");
            exit(EXIT_FAILURE); 
        }

        /* DEBUG */
        printf(">>> FD_ISSET = %d\n", FD_ISSET(fd, &rfds));

        if (FD_ISSET(fd, &rfds)) {
            /* DEBUG */
            printf(">>> server: ECHO4\n");

            addrlen = sizeof(addr);
            if ((newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) == -1) {
                perror("ERROR: accept\n");
                exit(EXIT_FAILURE);
            }

            switch(state) {
                case idle: 
                    /* DEBUG */
                    printf(">>> server: ECHO5\n");

                    afd = newfd; 
                    state = busy; 
                    break;
                case busy: /*  */ /* write "busy\n" in newfd */ 
                    /* DEBUG */
                    printf(">>> server: ECHO6\n");

                    close(newfd); 
                    break; // ???
            }
        }

        /* DEBUG */
        printf(">>> server: ECHO7\n");

        if (FD_ISSET(afd, &rfds)) {
            if ((n == read(afd, buffer, 128)) != 0) {
                /* DEBUG */
                printf(">>> server: ECHO8\n");

                if (n == -1) {
                    perror("ERROR: read\n");
                    exit(EXIT_FAILURE);
                }
                /*  */ // write buffer in afd
                /* DEBUG */
                printf(">>> server: ECHOOo\n");
            }
            else {
                /* DEBUG */
                printf(">>> server: ECHO9\n");

                close(afd);
                state = idle;
            }
        } // while (1)
        /* close(fd); exit(0); */
    }

    /* char command[MAX_SIZE];
    char keyword[MAX_SIZE];
    strcpy(message, "");

    validate_program_input(argc, argv);

    while(1) {
        fgets(command, MAX_SIZE, stdin);
        get_first_token(command, keyword);
        
        if (strcmp(keyword, "reg") == 0) {
            register_command(command);
        }
        else if (strcmp(keyword, "unregister") == 0 || strcmp(keyword, "unr") == 0) {
            unregister_command(command);
        }
        else if (strcmp(keyword, "login") == 0) {
            login_command(command);
        }
        else if (strcmp(keyword, "logout") == 0) {
            logout_command(command);
        }
        else if (strcmp(keyword, "showuid") == 0 || strcmp(keyword, "su") == 0) {
            showuid_command();
        }
        else if (strcmp(keyword, "exit") == 0) {
            exit_command(command);
        }
        else if (strcmp(keyword, "groups") == 0 || strcmp(keyword, "gl") == 0) {
            groups_command(command);
        }
        else if (strcmp(keyword, "subscribe") == 0 || strcmp(keyword, "s") == 0) {
            subscribe_command(command);
        }
        else if (strcmp(keyword, "unsubscribe") == 0 || strcmp(keyword, "u") == 0) {
            unsubscribe_command(command);
        }
        else if (strcmp(keyword, "my_groups") == 0 || strcmp(keyword, "mgl") == 0) {
            my_groups_command(command);
        }
        else if (strcmp(keyword, "select") == 0 || strcmp(keyword, "sag") == 0) {
            select_command(command);
        }
        else if (strcmp(keyword, "showgid") == 0 || strcmp(keyword, "sg") == 0) {
            showgid_command();
        }
        else if (strcmp(keyword, "ulist") == 0 || strcmp(keyword, "ul") == 0) {
            ulist_command();
        }
        else if (strcmp(keyword, "post") == 0) {
            post_command(command);
        }
        else if (strcmp(keyword, "retrieve") == 0 || strcmp(keyword, "r") == 0) {
            retrieve_command(command);
        }
        else {
            fprintf(stderr, "> Invalid command.\n");
            continue;
        }
    } */
    return 0;
}


