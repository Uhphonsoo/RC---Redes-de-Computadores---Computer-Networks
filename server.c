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

    int strmfd, dgrmfd, newfd;
    struct sockadrr_in servstrmaddr, servdgrmaddr, clientaddr;
    int len;
    socklen_t clientlen;
    fd_set testmask, mask;

    strmfd = create_socket();

    return 0;
}


