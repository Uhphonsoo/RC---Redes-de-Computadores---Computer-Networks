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
#define MAX_SIZE_GROUPS 5000 /* 7+99*(3+24+6)*sizeof(char) */
#define PORT_CONST 58000
#define FENIX_GROUP_NUMBER 20
/* #define MAX_PASS_SIZE 8 */

// TODO
/* 
 * improve invalid input messages
 */

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
void register_command(char*);
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
void send_and_receive(char*, char*);
void validate_sendto(int);
void validate_recvfrom(int);
int  validate_UID(char*);
int  validate_pass(char*);
int  validate_GID(char*);
int validate_GName(char*);
int  validate_registration_command(char*, char*, char*);
int  validate_login_command(char*, char*, char*);
int  validate_logout_command(char*);
int  validate_exit_command(char*);
int  validate_groups_command(char*);
int  validate_subscribe_command(char*, char*, char*);
void validate_register_reply(char*, char*, char*);
void validate_unregister_reply(char*,char*, char*);
void validate_login_reply(char*, char*, char*);
void validate_logout_reply(char*, char*, char*);
int  validate_groups_reply(char*, char*, char*);
int validate_subscribe_reply(char*, char*, char*);
void show_groups(char*, char*);
int  is_empty_string(char*);
void clear_string(char*);
void terminate_string(char*);
void close_TCP_connections();


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
        exit(EXIT_FAILURE);
    }


    while(1) {
        fgets(command, MAX_SIZE, stdin);
        get_first_token(command, keyword);

        /* DEBUG */
        /* printf("command: %s\n", command); */
        
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
        else if (strcmp(keyword, "exit") == 0) {
            exit_command(command);
        }
        else if (strcmp(keyword, "login") == 0) {
            login_command(command);
        }
        else if (strcmp(keyword, "groups") == 0 || strcmp(keyword, "gl") == 0) {
            groups_command(command);
        }
        else if (strcmp(keyword, "subscribe") == 0 || strcmp(keyword, "s") == 0) {
            subscribe_command(command);
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

void register_command(char* command) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];

    sscanf(command, "%s %s %s", aux, UID, pass);
    if(!validate_registration_command(command, UID, pass)) {
        return;
    }

    sprintf(message, "REG %s %s\n", UID, pass);

    // communication with server
    send_and_receive(message, reply);
    terminate_string(reply);

    sscanf(reply, "%s %s", aux, status);

    validate_register_reply(reply, aux, status);
    return;
}

void unregister_command(char* command) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];

    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_registration_command(command, UID, pass)) {
        return;
    }

    sprintf(message, "UNR %s %s\n", UID, pass);

    // communication with server
    send_and_receive(message, reply);
    terminate_string(reply);

    sscanf(reply, "%s %s", aux, status);

    validate_unregister_reply(reply, aux, status);
    return;
}

void login_command(char* command) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];

    if (logged_in) {
        printf("> A user is already logged in.\n");
        return;
    }

    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_login_command(command, UID, pass)) {
        return;
    }

    strcpy(logged_in_UID, UID);
    strcpy(logged_in_pass, pass);

    sprintf(message, "LOG %s %s\n", UID, pass);

    // communication with server
    send_and_receive(message, reply);
    terminate_string(reply);

    sscanf(reply, "%s %s", aux, status);

    validate_login_reply(reply, aux, status);
    return;
}

void logout_command(char* command) {

    char aux[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];

    if (is_empty_string(logged_in_UID) && is_empty_string(logged_in_pass)) {
        printf("> No user is currently logged in.\n");
        return;
    }
    
    sscanf(command, "%s", aux);
    if (!validate_logout_command(command)) {
        return;
    }

    sprintf(message, "OUT %s %s\n", logged_in_UID, logged_in_pass);

    // communication with server
    send_and_receive(message, reply);
    terminate_string(reply);

    sscanf(reply, "%s %s", aux, status);

    validate_logout_reply(reply, aux, status);
    return;
}

void exit_command(char* command) {

    char aux[MAX_SIZE];
    ssize_t n;

    sscanf(command, "%s", aux);

    if (!validate_exit_command(command)) {
        return;
    }

    close_TCP_connections();
    exit(EXIT_SUCCESS);
}

void groups_command(char* command) {

    char aux[MAX_SIZE];
    char N[MAX_SIZE];
    //char UID[MAX_SIZE];
    //char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_GROUPS];
    //char status[MAX_SIZE];

    sscanf(command, "%s", aux);
    if(!validate_groups_command(command)) {
        return;
    }

    sprintf(message, "GLS\n");

    // communication with server
    send_and_receive(message, reply);
    terminate_string(reply);

    sscanf(reply, "%s %s", aux, N);

    validate_groups_reply(reply, aux, N);
    show_groups(reply, N);
    
    return;
}

void subscribe_command(char* command) {

    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE];
    char status[MAX_SIZE];

    sscanf(command, "%s %s %s", aux, GID, GName);
    if(!validate_subscribe_command(command, GID, GName)) {
        return;
    }

    sprintf(message, "GSR %s %s %s\n", logged_in_UID, GID, GName);

    /* DEBUG */
    printf("Message = %s\n", message);
    printf("K");

    // communication with server
    send_and_receive(message, reply);
    terminate_string(reply);

    sscanf(reply, "%s %s", aux, status);

    /* DEBUG */
    printf("Reply = %s\n", reply);
    printf("L");

    validate_subscribe_reply(reply, aux, status);
    return;
}

void unsubscribe_command(char* command) {
    // TODO
    return;
}

void my_groups_command(char* command) {
    // TODO
    return;
}

void select_command(char* command) {
    // TODO
    return;
}

void post_command(char* command) {
    // TODO
    return;
}

void retrieve_command(char* command) {
    // TODO
    return;
}

void get_first_token(char* string, char* ret) {

    int i = 0;

    while(!isspace(string[i])) {
        ret[i] = string[i];
        i++;
    }
    
    ret[i] = '\0';
    return;
}

void get_nth_token(char* string, int n, char* ret) {

    int i = 0;
    int j = 1;

    if (n == 0) {
        fprintf(stderr, "ERROR: get_nth_token(): invalid input.\n");
    }

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
        fprintf(stderr, "ERROR: get_nth_token(): string doesn't have that many tokens.\n");
        exit(EXIT_FAILURE);
    }
    return;
}

int get_number_of_tokens(char* string) {

    int ret = 0;
    int length = strlen(string);
    int last_read_character_was_space = 0;

    if (string == NULL) {
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (isspace(string[i])) {
            if (!last_read_character_was_space) {
                ret++;
            }
            last_read_character_was_space = 1;
        }
        else {
            last_read_character_was_space = 0;
        }
    }
    return ret;
}

int validate_registration_command(char* command, char* UID, char* pass) {

    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 3) {
        fprintf(stderr, "> (unr)registration: Wrong number of arguments in input.\n");
        return 0;
    }
    if (!validate_UID(UID)) {
        fprintf(stderr, "> (unr)registration: Invalid user ID.\n");
        return 0;
    }
    if (!validate_pass(pass)) {
        fprintf(stderr, "> (unr)registration: Invalid user password.\n");
        return 0;
    }
    return 1;
}

void validate_register_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if ((number_of_tokens_reply != 2) || strcmp("RRG", aux)) {
        fprintf(stderr, "> register_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0) {
        printf("> User successfully registered.\n");
    }
    else if (strcmp(status, "DUP") == 0) {
        printf("> Failed to register user. User already registered.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("> Failed to register user.\n");
    }
    else {
        fprintf(stderr, "> ERROR: register_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    return;
}

void validate_unregister_reply(char* reply,char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if ((number_of_tokens_reply != 2) || strcmp("RUN", aux)) {
        fprintf(stderr, "> unregister_command: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0) {
        printf("> User successfully unregistered.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("> Failed to unregister user.\n");
    }
    else {
        fprintf(stderr, "> unregister_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    return;
}

int validate_login_command(char* command, char* UID, char* pass) {

    int number_of_tokens_command = get_number_of_tokens(command);
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

void validate_login_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if ((number_of_tokens_reply != 2) || strcmp("RLO", aux)) {
        fprintf(stderr, "login_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0) {
        logged_in = 1;
        printf("> User successfully logged in.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("> Failed to login. User ID or password not valid.\n");
    }
    else {
        fprintf(stderr, "ERROR: login_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    return;
}

int validate_logout_command(char* command) {
    
    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 1) {
        fprintf(stderr, "> logout_command: Invalid input.\n");
        return 0;
    }
    return 1;
}

void validate_logout_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (number_of_tokens_reply != 2 || strcmp("ROU", aux)) {
        fprintf(stderr, "ERROR: logout_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0) {
        clear_string(logged_in_UID);
        clear_string(logged_in_pass);
        logged_in = 0;
        printf("> User successfully logged out.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("> Failed to logout. User ID or password not valid.\n");
    }
    else {
        fprintf(stderr, "logout_command(): ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    return;
}

int validate_exit_command(char* command) {

    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 1) {
        fprintf(stderr, "> exit_command: Invalid input.\n");
        return 0;
    }

    return 1;
}

int validate_groups_command(char* command) {

    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 1) {
        fprintf(stderr, "> validate_groups_command: Invalid input.\n");
        return 0;
    }
    return 1;
}

int validate_groups_reply(char* reply, char* aux, char* N) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (number_of_tokens_reply < 2) {
        fprintf(stderr, "> validate_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (number_of_tokens_reply == 2 && (strcmp("RGL", aux) || strcmp(N, "0"))) {
        fprintf(stderr, "> validate_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

int validate_subscribe_command(char* command, char* GID, char* GName) {

    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 3) {
        fprintf(stderr, "> validate_subscribe_command: Invalid input.\n");
        return 0;
    }
    if (!validate_GID(GID)) {
        fprintf(stderr, "validate_subscribe_command: Invalid group ID.\n");
        return 0;
    }
    if (!validate_GName(GName)) {
        fprintf(stderr, "validate_subscribe_command: Invalid group name.\n");
        return 0;
    }
    
    return 1;
}

int validate_subscribe_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_command = get_number_of_tokens(reply);
    if (number_of_tokens_command != 2 || strcmp(aux, "RGS")) {
        fprintf(stderr, "> validate_subscribe_reply: ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0) {
        printf("> Successfully subscribed to group.\n");
    }
    else if (strcmp(status, "NEW GID") == 0) {
        printf("> Successfully created new group and subscribed to it.\n");
    }
    else if (strcmp(status, "E_USR") == 0) {
        printf("> Invalid user ID.\n");
    }
    else if (strcmp(status, "E_GRP") == 0) {
        printf("> Invalid group ID.\n");
    }
    else if (strcmp(status, "E_GNAME") == 0) {
        printf("> Invalid group name.\n");
    }
    else if (strcmp(status, "E_FULL") == 0) {
        printf("> Could not create group. Group limit reached.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("> Failed to subscribe to group.\n");
    }
    else {
        fprintf(stderr, "> ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
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
        fprintf(stderr, "> Invalid user password.\n");
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (!(isalpha(pass[i]) || isdigit(pass[i]))) {
            // printf TODO
            return 0;
        }
    }
    return 1;
}

int validate_GID(char* GID) {

    int length = strlen(GID);

    if (strlen(GID) != 2) {
        fprintf(stderr, "> Invalid group ID.");
        return 0;
    }
    for (int i = 0; i < length; i++) {
        if (!isdigit(GID[i])) {
            return 0;
        }
    }
    return 1;
}

int validate_GName(char* GName) {

    int length = strlen(GName);

    if (strlen(GName) == 0 || strlen(GName) > 24) {
        fprintf(stderr, "> Invalid group name.");
        return 0;
    }
    for (int i = 0; i < length; i++) {
        if (!(isalpha(GName[i]) || isdigit(GName[i]) || GName[i] != '-' || GName[i] != '_')) {
            return 0;
        }
    }
    return 1;
}

void show_groups(char* reply, char* N_string) {

    int N = atoi(N_string);
    int i = 0;
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];

    for (i = 3; i < 3*N + 1; i++) {
        get_nth_token(reply, i++, GID);
        get_nth_token(reply, i++, GName);

        printf("> Group ID: %s | Group Name: %s\n", GID, GName);
    }

    return;
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

void send_and_receive(char* message, char* reply){

    int n = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    validate_sendto(n);

    addrlen = sizeof(addr);
    n = recvfrom(fd, reply, MAX_SIZE_GROUPS, 0, (struct sockaddr*)&addr, &addrlen);
    validate_recvfrom(n);
}