#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/* #include <errno.h> */
/* #include <sys/types.h> */
#include <sys/socket.h>
#include <netdb.h> 
#include <ctype.h>
#define MAX_SIZE 240
#define PORT_CONST 58000
#define PORT_CONST_STRING itoa(PORT_CONST)
#define FENIX_GROUP_NUMBER 20
#define MAX_PASS_SIZE 8

// registration commands
void reg_command(char*);
void unregister_command(char*);
// user identification and session termination commands
void login_command(char*);
void logout_command(char*);
void exit_command(char*);
// group management commads
void groups_command(char*);
void subscribe_command(char*);
void unsubscribe_command(char*);
void my_groups_command(char*);
void select_command(char*);
// messaging commands
void post_command(char*);
void retrieve_command(char*);
// other
void get_DSIP(char*, char*);
void get_first_word(char*, char*);
void get_nth_word(char*, int, char*);

// Global variables
int fd;
int UID;
char pass[MAX_PASS_SIZE];
char DSIP[MAX_SIZE];
char DSport[MAX_SIZE];
int errcode;
/* ssize_t n; */
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
/* char buffer[128]; */


int main(int argc, char *argv[]) {

    int GN = 1;
    char command[MAX_SIZE];
    char keyword[MAX_SIZE];

    if (argc == 1) {
        gethostname(DSIP, MAX_SIZE);
        sprintf(DSport, "%d", PORT_CONST + FENIX_GROUP_NUMBER);
    }
    else if (argc == 5) {
        if (strcmp(argv[1], "-n")){
            fprintf(stderr, "ERROR: Invalid input. Input should be ./user -n [DSIP] -p [DSport].\n");
            exit(EXIT_FAILURE);
        }
        if (strcmp(argv[3], "-p")){
            fprintf(stderr, "ERROR: Invalid input. Input should be ./user -n [DSIP] -p [DSport].\n");
            exit(EXIT_FAILURE);
        }
        strcpy(DSIP, argv[2]);
        strcpy(DSport, argv[4]);
    }
    else {
        fprintf(stderr, "ERROR: Invalid input. Input should be ./user -n [DSIP] -p [DSport].\n");
        exit(EXIT_FAILURE);
    }

    /* get_DSIP(arg1, DSIP); */
    /* get_DSport(arg2, ) */
    
    /* DEBUG */
    /* printf("DSIP: %s\n", DSIP);
    printf("DSport: %s\n", DSport); */

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
    if(fd == -1) {
        perror("ERROR: socket(): can't open socket.\n");
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_DGRAM;   // UDP socket

    errcode = getaddrinfo(DSIP, DSport, &hints, &res);
    if(errcode != 0) {
        perror("ERROR: getaddrinfo()\n");
        exit(1);
    }


    while(1) {
        fgets(command, MAX_SIZE, stdin);
        get_first_word(command, keyword);

        /* DEBUG */
        /* printf("command: %s\n", command); */
        
        if (strcmp(keyword, "reg") == 0) {
            reg_command(command);
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
        else if (strcmp(keyword, "exit") == 0) {
            exit_command(command);
        }
        else if (strcmp(keyword, "login") == 0) {
            login_command(command);
        }
        else if (strcmp(keyword, "groups") == 0) {
            groups_command(command);
        }
        else if (strcmp(keyword, "unsubscribe") == 0) {
            unsubscribe_command(command);
        }
        else if (strcmp(keyword, "my_groups") == 0) {
            my_groups_command(command);
        }
        else if (strcmp(keyword, "select") == 0) {
            select_command(command);
        }
        else if (strcmp(keyword, "post") == 0) {
            post_command(command);
        }
        else if (strcmp(keyword, "retrieve") == 0) {
            retrieve_command(command);
        }
        else {
            fprintf(stderr, "ERROR: invalid command.\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void reg_command(char* command) {

    ssize_t n;
    char message[MAX_SIZE] = "";
    char UID_string[MAX_SIZE];
    char reply[MAX_SIZE];
    char status[MAX_SIZE];

    get_nth_word(command, 2, UID_string);
    UID = atoi(UID_string);
    get_nth_word(command, 3, pass);

    sprintf(message, "REG %s %s\n", UID_string, pass);

    n = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    if(n == -1) {
        perror("ERROR: sendto().\n");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(addr);
    n = recvfrom(fd, reply, MAX_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        perror("ERROR: reg_command(): recvfrom().\n");    
        exit(EXIT_FAILURE);
    } 

    /* DEBUG */
    printf("servidor: %s\n", reply);

    get_nth_word(reply, 2, status);

    if (strcmp(status, "OK") == 0) {
        printf("User successfully registered.\n");
    }
    else if (strcmp(status, "DUP") == 0) {
        printf("Failed to register user. User already registered.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("Failed to register user.\n");
    }

    return;
}

void unregister_command(char* command) {
    return;
}

void login_command(char* command) {
    return;
}

void logout_command(char* command) {
    return;
}

void exit_command(char* command) {
    return;
}

void groups_command(char* command) {
    return;
}

void subscribe_command(char* command) {
    return;
}

void unsubscribe_command(char* command) {
    return;
}

void my_groups_command(char* command) {
    return;
}

void select_command(char* command) {
    return;
}

void post_command(char* command) {
    return;
}

void retrieve_command(char* command) {
    return;
}

void get_first_word(char* string, char* ret) {
    int i = 0;

    while(!isspace(string[i])) {
        ret[i] = string[i];
        i++;
    }
    ret[i] = '\0';
}

void get_nth_word(char* string, int n, char* ret) {

    if (n == 0) {
        fprintf(stderr, "ERROR: get_nth_word(): invalid input.\n");
    }
    
    int i = 0;
    int j = 1;

    while(j != n) {
        if (isspace(string[i])) {
            j++;
        }
        i++;
    }

    int k = 0;
    while(!isspace(string[i])) {
        ret[k++] = string[i++];
    }
    ret[k] = '\0';

    if (strcmp(ret, "") == 0) {
        fprintf(stderr, "ERROR: get_nth_word(): string doesn't have that many words.\n");
        exit(EXIT_FAILURE);
    }
}

void get_DSIP(char* command, char* ret) {
    int i = 0, j = 0;

    while(command[i] != 'n') {
        i++;
    }
    i += 2;

    while(!(isspace(command[i]))) {
        ret[j++] = command[i++];
    }
    ret[j] = '\0';
}