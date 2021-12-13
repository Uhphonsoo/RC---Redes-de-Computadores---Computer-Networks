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
#define FENIX_GROUP_NUMBER 20
/* #define MAX_PASS_SIZE 8 */

// Global variables
int fd;
int errcode;
int logged_in;
/* int UID; */
/* char pass[MAX_PASS_SIZE]; */
char DSIP[MAX_SIZE];
char DSport[MAX_SIZE];
char logged_in_UID[MAX_SIZE];
char logged_in_pass[MAX_SIZE];
/* ssize_t n; */
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
/* char buffer[128]; */

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

// misc
/* void get_DSIP(char*, char*); */
void get_first_token(char*, char*);
void get_nth_token(char*, int, char*);
int  get_number_of_tokens(char*);
void validate_sendto(int);
void validate_recvfrom(int);
int  validate_registration_command(int, char*, char*);
int  validate_login_command(int, char*, char*);
int  validate_logout_command(int);
int  validate_exit_command(int);
int  validate_UID(char*);
int  validate_pass(char*);
int  is_empty_string(char*);
void clear_string(char*);
void terminate_string(char*);
void close_TCP_connections();
void send_and_receive(char*, char*);


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
            fprintf(stderr, "ERROR: Invalid input. Input has the form ./user -n [DSIP] -p [DSport].\n");
            exit(EXIT_FAILURE);
        }
        if (strcmp(argv[3], "-p")){
            fprintf(stderr, "ERROR: Invalid input. Input has the form ./user -n [DSIP] -p [DSport].\n");
            exit(EXIT_FAILURE);
        }
        strcpy(DSIP, argv[2]);
        strcpy(DSport, argv[4]);
    }
    else {
        fprintf(stderr, "ERROR: Invalid input. Input has the form ./user -n [DSIP] -p [DSport].\n");
        exit(EXIT_FAILURE);
    }

    /* get_DSIP(arg1, DSIP); */
    /* get_DSport(arg2, ) */
    
    /* DEBUG */
    /* printf("DSIP: %s\n", DSIP);
    printf("DSport: %s\n", DSport); */

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
    if(fd == -1) {
        perror("ERROR: socket: can't open socket.\n");
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_DGRAM;   // UDP socket

    errcode = getaddrinfo(DSIP, DSport, &hints, &res);
    if(errcode != 0) {
        perror("ERROR: getaddrinfo\n");
        exit(1);
    }


    while(1) {
        fgets(command, MAX_SIZE, stdin);
        get_first_token(command, keyword);

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
            fprintf(stderr, "Invalid command.\n");
            continue;
        }
    }

    return 0;
}

void reg_command(char* command) {

    int number_of_tokens_command = 0;
    int number_of_tokens_reply = 0;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];
    ssize_t n;

    /* DEBUG */
    /* printf("command: %s\n", command); */

    number_of_tokens_command = get_number_of_tokens(command);
    sscanf(command, "%s %s %s", aux, UID, pass);

    /* DEBUG */
    /* printf("command: %s\n", command);
    printf("UID: %s\n", UID);
    printf("pass_: %s\n", pass); */

    if(!validate_registration_command(number_of_tokens_command, UID, pass)) {
        return;
    }

    sprintf(message, "REG %s %s\n", UID, pass);

    // Server comunication
    send_and_receive(message, reply);

    number_of_tokens_reply = get_number_of_tokens(reply);
    sscanf(reply, "%s %s", aux, status);

    /* DEBUG */
    printf("reply: %s\n", reply);

    if (number_of_tokens_reply != 2) {
        fprintf(stderr, "ERROR: reg_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp("RRG", aux)) {
        fprintf(stderr, "ERROR: reg_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    /* get_nth_token(reply, 2, status); */

    if (strcmp(status, "OK") == 0) {
        printf("User successfully registered.\n");
    }
    else if (strcmp(status, "DUP") == 0) {
        printf("Failed to register user. User already registered.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("Failed to register user.\n");
    }
    else {
        fprintf(stderr, "ERROR: reg_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    return;
}

void unregister_command(char* command) {

    int number_of_tokens_command = 0;
    int number_of_tokens_reply = 0;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];
    ssize_t n;

    number_of_tokens_command = get_number_of_tokens(command);
    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_registration_command(number_of_tokens_command, UID, pass)) {
        return;
    }

    sprintf(message, "UNR %s %s\n", UID, pass);

    // Server comunication
    send_and_receive(message, reply);

    number_of_tokens_reply = get_number_of_tokens(reply);
    sscanf(reply, "%s %s", aux, status);

    terminate_string(reply);

    /* DEBUG */
    /* printf("reply: %s\n", reply);
    printf("number_of_tokens_reply: %d\n", number_of_tokens_reply);
    printf("aux: %s\n", aux); */

    /* DEBUG */
    printf("reply: %s\n", reply);
    for (int i = 0; i < strlen(reply); i++) {
        printf("- %c\n", reply[i]);
    }

    if (number_of_tokens_reply != 2) {
        fprintf(stderr, "ERROR: (unr)egister_command: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp("RUN", aux)) {
        fprintf(stderr, "ERROR: (unr)egister_command: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0) {
        printf("User successfully unregistered.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("Failed to unregister user.\n");
    }
    else {
        fprintf(stderr, "ERROR: (unr)egister_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    return;
}

void login_command(char* command) {

    int number_of_tokens_command = 0;
    int number_of_tokens_reply = 0;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];
    ssize_t n;

    if (logged_in) {
        printf("[-] Already logged in.\n");
        return;
    }

    number_of_tokens_command = get_number_of_tokens(command);
    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_login_command(number_of_tokens_command, UID, pass)) {
        return;
    }

    strcpy(logged_in_UID, UID);
    strcpy(logged_in_pass, pass);

    /* DEBUG */
    printf("logged_in_UID: %s\n", logged_in_UID);
    printf("logged_in_pass: %s\n", logged_in_pass);

    sprintf(message, "LOG %s %s\n", UID, pass);

    // Server comunication
    send_and_receive(message, reply);

    number_of_tokens_reply = get_number_of_tokens(reply);
    sscanf(reply, "%s %s", aux, status);

    if (number_of_tokens_reply != 2) {
        fprintf(stderr, "ERROR: login_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp("RLO", aux)) {
        fprintf(stderr, "ERROR: login_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    /* DEBUG */
    printf("servidor: %s\n", reply);

    if (strcmp(status, "OK") == 0) {
        logged_in = 1;
        printf("User successfully logged in.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("Failed to login. User ID or password not valid.\n");
    }
    else {
        fprintf(stderr, "ERROR: login_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    return;
}

void logout_command(char* command) {

    int number_of_tokens_command = 0;
    int number_of_tokens_reply = 0;
    char aux[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];
    ssize_t n;

    if (is_empty_string(logged_in_UID) && is_empty_string(logged_in_pass)) {
        printf("No user is currently logged in.\n");
        return;
    }
    
    number_of_tokens_command = get_number_of_tokens(command);
    sscanf(command, "%s", aux);
    if (!validate_logout_command(number_of_tokens_command)) {
        return;
    }

    sprintf(message, "OUT %s %s\n", logged_in_UID, logged_in_pass);

    // n = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    // validate_sendto(n);

    // addrlen = sizeof(addr);
    // n = recvfrom(fd, reply, MAX_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
    // validate_recvfrom(n);
    send_and_receive(message, reply);

    number_of_tokens_reply = get_number_of_tokens(reply);
    sscanf(reply, "%s %s", aux, status);

    /* terminate_string(reply); */

    if (number_of_tokens_reply != 2) {
        fprintf(stderr, "ERROR: logout_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp("ROU", aux)) {
        fprintf(stderr, "ERROR: logout_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    /* DEBUG */
    printf("servidor: %s\n", reply);

    if (strcmp(status, "OK") == 0) {
        clear_string(logged_in_UID);
        clear_string(logged_in_pass);
        logged_in = 0;
        printf("User successfully logged out.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("Failed to logout. User ID or password not valid.\n");
    }
    else {
        fprintf(stderr, "ERROR: logout_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    /* DEBUG */
    printf("logged_in_UID: %s\n", logged_in_UID);
    printf("logged_in_pass: %s\n", logged_in_pass);
    
    return;
}

void exit_command(char* command) {

    int number_of_tokens_command = 0;
    char aux[MAX_SIZE];
    ssize_t n;

    number_of_tokens_command = get_number_of_tokens(command);
    sscanf(command, "%s", aux);

    if (!validate_exit_command(number_of_tokens_command)) {
        return;
    }

    close_TCP_connections();
    exit(EXIT_SUCCESS);
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

void get_first_token(char* string, char* ret) {
    int i = 0;

    while(!isspace(string[i])) {
        ret[i] = string[i];
        i++;
    }
    ret[i] = '\0';
}

void get_nth_token(char* string, int n, char* ret) {

    if (n == 0) {
        fprintf(stderr, "ERROR: get_nth_token(): invalid input.\n");
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
        fprintf(stderr, "ERROR: get_nth_token(): string doesn't have that many words.\n");
        exit(EXIT_FAILURE);
    }
}

int get_number_of_tokens(char* string) {
    int ret = 0;
    int length = strlen(string);

    if (string == NULL) {
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (isspace(string[i])) {
            ret++;
        }
    }

    return ret;
}

int validate_registration_command(int number_of_tokens_command, char* UID, char* pass) {
    if (number_of_tokens_command != 3) {
        fprintf(stderr, "(unr)registration: Wrong number of arguments in input.\n");
        return 0;
    }
    if (!validate_UID(UID)) {
        fprintf(stderr, "(unr)registration: Invalid user ID.\n");
        return 0;
    }
    if (!validate_pass(pass)) {
        fprintf(stderr, "(unr)registration: Invalid user password.\n");
        return 0;
    }

    return 1;
}

int validate_login_command(int number_of_tokens_command, char* UID, char* pass) {
    if (number_of_tokens_command != 3) {
        fprintf(stderr, "login_command: Wrong number of arguments in input.\n");
        return 0;
    }
    if (!validate_UID(UID)) {
        fprintf(stderr, "login_command: Invalid user ID.\n");
        return 0;
    }
    if (!validate_pass(pass)) {
        fprintf(stderr, "login_command: Invalid user password.\n");
        return 0;
    }

    return 1;
}

int validate_logout_command(int number_of_tokens_command) {
    if (number_of_tokens_command != 1) {
        fprintf(stderr, "logout_command: Invalid input.\n");
        return 0;
    }

    return 1;
}

int validate_exit_command(int number_of_tokens_command) {
    if (number_of_tokens_command != 1) {
        fprintf(stderr, "exit_command: Invalid input.\n");
        return 0;
    }

    return 1;
}

void validate_sendto(int n) {
    if(n == -1) {
        perror("ERROR: sendto(): Unable to send message.\n");
        exit(EXIT_FAILURE);
    }
}

void validate_recvfrom(int n) {
    if(n == -1) {
        perror("ERROR: recvfrom(): Unable to receive message.\n");
        exit(EXIT_FAILURE);
    }
}

int validate_UID(char* UID) {
    int length = strlen(UID);

    if (length != 5) {
        fprintf(stderr, "Invalid user ID.\n");
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (!isdigit(UID[i])) {
            return 0;
        }
    }

    return 1;
}

int validate_pass(char* pass) {
    int length = strlen(pass);

    if (strlen(pass) != 8) {
        fprintf(stderr, "Invalid user password.\n");
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (!(isalpha(pass[i]) || isdigit(pass[i]))) {
            return 0;
        }
    }

    return 1;
}

void clear_string(char* string) {
    if (string != NULL) {
        string[0] = '\0';
    }

    return;
}

void terminate_string(char* string) {
    int length = strlen(string);
    string[length] = '\0';

    return;
} 

int is_empty_string(char* string) {
    if (string == NULL) {
        return 1;
    }
    
    return string[0] == '\0';
}

void close_TCP_connections() {
    // TODO
    return;
}

/* void get_DSIP(char* command, char* ret) {
    int i = 0, j = 0;

    while(command[i] != 'n') {
        i++;
    }
    i += 2;

    while(!(isspace(command[i]))) {
        ret[j++] = command[i++];
    }
    ret[j] = '\0';
} */

void send_and_receive(char* message, char* reply){

    int n = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    validate_sendto(n);

    addrlen = sizeof(addr);
    n = recvfrom(fd, reply, MAX_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
    validate_recvfrom(n);
}