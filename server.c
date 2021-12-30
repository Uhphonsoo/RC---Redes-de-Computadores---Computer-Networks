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


int main(int argc, char *argv[]) {

    /* char command[MAX_SIZE];
    char keyword[MAX_SIZE];
    strcpy(message, ""); */

    int n = 0;
    socklen_t addrlen;
    pid_t child_pid;

    create_server_socket_TCP();
    get_address_info_server_TCP();

    n = bind(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    // TODO: validate_bind(n)
    if(n == -1) {
        perror("ERROR: bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(fd_TCP, 5) == -1) {
        perro("ERROR: listen\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        addrlen = sizeof(addr);

        newfd = accept(fd, (struct sockaddr*)&addr, &addrlen);
        validate_accept(newfd);

        child_pid = fork();
        if (child_pid == 0) { // child process
            close(fd_TCP);

            while(1) {
                n = read(newfd, message, MAX_SIZE);
                validate_read(n);

                /* TODO: interpret message
                         send reply */
            }
        }
        close(newfd);
    }





    /*
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


