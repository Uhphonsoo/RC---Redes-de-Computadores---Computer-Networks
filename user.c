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
#define MAX_SIZE_REPLY 5000 /* 7+99*(3+24+6)*sizeof(char) */
#define MAX_FILE_SIZE 1024
#define PORT_CONST 58000
#define FENIX_GROUP_NUMBER 20
/* #define MAX_PASS_SIZE 8 */

// TODO
/* 
 * improve invalid input messages
 */

// Global variables
int  fd_UDP, fd_TCP;
int  errcode;
int  logged_in;
int  has_active_group;
char DSIP[MAX_SIZE];
char DSport[MAX_SIZE];
char logged_in_UID[MAX_SIZE];
char logged_in_pass[MAX_SIZE];
char active_GID[MAX_SIZE];
/* ssize_t n; */
socklen_t addrlen_UDP, addrlen_TCP;
struct addrinfo hints_UDP, *res_UDP, hints_TCP, *res_TCP;
struct sockaddr_in addr_UDP, addr_TCP;
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
int  validate_registration_command(char*, char*, char*);
int  validate_login_command(char*, char*, char*);
int  validate_logout_command(char*);
int  validate_exit_command(char*);
int  validate_groups_command(char*);
int  validate_subscribe_command(char*, char*, char*);
int  validate_unsubscribe_command(char*, char*);
int  validate_my_groups_command(char*);
int  validate_select_command(char*, char*);
int  validate_post_command(char*, char*, char*, char*, int*);
void validate_register_reply(char*, char*, char*);
void validate_unregister_reply(char*,char*, char*);
void validate_login_reply(char*, char*, char*);
void validate_logout_reply(char*, char*, char*);
void validate_groups_reply(char*, char*, char*);
void validate_subscribe_reply(char*, char*, char*);
void validate_usubscribe_reply(char*, char*, char*);
void validate_my_groups_reply(char*, char*, char*);
void validate_post_reply(char*, char*, char*);
void validate_program_input(int, char**);
void validate_sendto(int);
void validate_recvfrom(int);
int  validate_UID(char*);
int  validate_pass(char*);
int  validate_GID(char*);
int  validate_GName(char*);
void create_UDP_socket();
void get_address_info_UDP();
void create_TCP_socket();
void get_address_info_TCP();
void get_first_token(char*, char*);
void get_nth_token(char*, int, char*);
int  get_number_of_tokens(char*);
int  get_file_size(FILE*);
void get_file_data(FILE*, char*);
int get_string_in_quotes(char* command, char* aux, char* text, char* file_name, int* file_is_being_sent);
void send_and_receive_UDP(char*, char*);
void send_and_receive_TCP(char*, char*);
void show_groups(char*, char*);
int  is_empty_string(char*);
void clear_string(char*);
// void terminate_string(char*);
void terminate_string_after_n_tokens(char*, int);
void close_TCP_connections();


int main(int argc, char *argv[]) {

    int GN = 1;
    char command[MAX_SIZE];
    char keyword[MAX_SIZE];

    // validate_program_input(argc, argv);

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

    // create_UDP_socket();

    fd_UDP = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
    if(fd_UDP == -1) {
        perror("ERROR: socket: can't open socket.\n");
        exit(EXIT_FAILURE);
    }

    // get_address_info_UDP();

    memset(&hints_UDP, 0, sizeof(hints_UDP)); 
    hints_UDP.ai_family = AF_INET;        // IPv4
    hints_UDP.ai_socktype = SOCK_DGRAM;   // UDP socket

    errcode = getaddrinfo(DSIP, DSport, &hints_UDP, &res_UDP);
    if(errcode != 0) {
        perror("ERROR: getaddrinfo\n");
        exit(EXIT_FAILURE);
    }

    create_TCP_socket();
    get_address_info_TCP();

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
        else if (strcmp(keyword, "unsubscribe") == 0 || strcmp(keyword, "u") == 0) {
            unsubscribe_command(command);
        }
        else if (strcmp(keyword, "my_groups") == 0 || strcmp(keyword, "mgl") == 0) {
            my_groups_command(command);
        }
        else if (strcmp(keyword, "select") == 0 || strcmp(keyword, "sag") == 0) {
            select_command(command);
        }
        else if (strcmp(keyword, "post") == 0) {
            post_command(command);
        }
        else if (strcmp(keyword, "retrieve") == 0) {
            retrieve_command(command);
        }
        else {
            fprintf(stderr, "> Invalid command.\n");
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
    char reply[MAX_SIZE_REPLY];
    char status[MAX_SIZE];

    sscanf(command, "%s %s %s", aux, UID, pass);
    if(!validate_registration_command(command, UID, pass)) {
        return;
    }

    sprintf(message, "REG %s %s\n", UID, pass);

    // communication with server
    send_and_receive_UDP(message, reply);
    terminate_string_after_n_tokens(reply, 2);

    sscanf(reply, "%s %s", aux, status);

    validate_register_reply(reply, aux, status);
    return;
}

void unregister_command(char* command) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_REPLY];
    char status[MAX_SIZE];

    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_registration_command(command, UID, pass)) {
        return;
    }

    sprintf(message, "UNR %s %s\n", UID, pass);

    // communication with server
    send_and_receive_UDP(message, reply);
    terminate_string_after_n_tokens(reply, 2);

    sscanf(reply, "%s %s", aux, status);

    validate_unregister_reply(reply, aux, status);
    return;
}

void login_command(char* command) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_REPLY];
    char status[MAX_SIZE];

    if (logged_in) {
        printf("> Failed to login. A user is already logged in.\n");
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
    send_and_receive_UDP(message, reply);
    terminate_string_after_n_tokens(reply, 2);

    sscanf(reply, "%s %s", aux, status);

    validate_login_reply(reply, aux, status);
    return;
}

void logout_command(char* command) {

    char aux[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_REPLY];
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
    send_and_receive_UDP(message, reply);
    terminate_string_after_n_tokens(reply, 2);

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

    freeaddrinfo(res_UDP);
    close(fd_UDP);
    freeaddrinfo(res_TCP);
    close(fd_TCP);

    exit(EXIT_SUCCESS);
}

void groups_command(char* command) {

    char aux[MAX_SIZE];
    char N[MAX_SIZE];
    //char UID[MAX_SIZE];
    //char pass[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_REPLY];
    //char status[MAX_SIZE];

    sscanf(command, "%s", aux);
    if(!validate_groups_command(command)) {
        return;
    }

    sprintf(message, "GLS\n");

    // communication with server
    send_and_receive_UDP(message, reply);

    sscanf(reply, "%s %s", aux, N);

    validate_groups_reply(reply, aux, N);

    if (strcmp(N, "0") == 0) {
        printf("> There are no available groups.\n");
        return;
    }

    show_groups(reply, N);
    
    return;
}

void subscribe_command(char* command) {

    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_REPLY];
    char status[MAX_SIZE];

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    sscanf(command, "%s %s %s", aux, GID, GName);
    if(!validate_subscribe_command(command, GID, GName)) {
        return;
    }

    sprintf(message, "GSR %s %s %s\n", logged_in_UID, GID, GName);

    /* DEBUG */
    /* printf("Subscribe: Message:%sT\n", message); */

    // communication with server
    send_and_receive_UDP(message, reply);
    /* terminate_string_after_n_tokens(reply, 3); */

    sscanf(reply, "%s %s", aux, status);

    /* DEBUG */
    /* printf("Subscribe: Reply:%sT\n", reply); */

    validate_subscribe_reply(reply, aux, status);
    return;
}

void unsubscribe_command(char* command) {

    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    // char GName[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_REPLY];
    char status[MAX_SIZE];

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    sscanf(command, "%s %s", aux, GID);
    if(!validate_unsubscribe_command(command, GID)) {
        return;
    }

    sprintf(message, "GUR %s %s\n", logged_in_UID, GID);

    // communication with server
    send_and_receive_UDP(message, reply);
    terminate_string_after_n_tokens(reply, 2);

    sscanf(reply, "%s %s", aux, status);

    validate_usubscribe_reply(reply, aux, status);
    return;
}

void my_groups_command(char* command) {

    char aux[MAX_SIZE];
    char N[MAX_SIZE];
    // char GID[MAX_SIZE];
    // char GName[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_REPLY];
    // char status[MAX_SIZE];

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    sscanf(command, "%s", aux);
    if(!validate_my_groups_command(command)) {
        return;
    }

    sprintf(message, "GLM %s\n", logged_in_UID);

    // communication with server
    send_and_receive_UDP(message, reply);
    /* terminate_string_after_n_tokens(reply, 2); */

    sscanf(reply, "%s %s", aux, N);

    validate_my_groups_reply(reply, aux, N);

    if (strcmp(N, "0") == 0) {
        printf("> Currently not subscribed to any group.\n");
        return;
    }

    show_groups(reply, N);
    return;
}


void select_command(char* command) {
    // TODO
    char aux[MAX_SIZE];
    // char N[MAX_SIZE];
    char GID[MAX_SIZE];
    // char GName[MAX_SIZE];
    char message[MAX_SIZE] = "";
    // char reply[MAX_SIZE_REPLY];
    // char status[MAX_SIZE];

    sscanf(command, "%s %s", aux, GID);
    if(!validate_select_command(command, GID)) {
        return;
    }

    /* sprintf(message, "GLM %s\n", logged_in_UID); */

    // communication with server
    /* send_and_receive_UDP(message, reply); */
    /* terminate_string_after_n_tokens(reply, 2); */

    /* sscanf(reply, "%s %s", aux, N);

    validate_my_groups_reply(reply, aux, N); */

    // TODO: Testar se GID existe???

    // set active group
    strcpy(active_GID, GID);
    has_active_group = 1;

    printf("> Group %s selected.\n", active_GID);
}

void post_command(char* command) {
    // TODO
    int file_is_being_sent = 0;
    int Tsize = 0;
    int Fsize = 0;
    char aux[MAX_SIZE];
    char text[MAX_SIZE];
    char Fname[MAX_SIZE];
    char *data = 0;
    // char GID[MAX_SIZE];
    // char GName[MAX_SIZE];
    char message[MAX_SIZE] = "";
    char reply[MAX_SIZE_REPLY];
    char status[MAX_SIZE];
    FILE *fp;

    // comentei oara ser mais facil testar. voltar a descomentar!
    /* if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }
    if (!has_active_group) {
        printf("> There is no active group.\n");
        return;
    } */

    sscanf(command, "%s", aux);
    if(!validate_post_command(command, aux, text, Fname, &file_is_being_sent)) {
        return;
    }

    /* DEBUG */
    /* printf(">> file is being sent: %d\n", file_is_being_sent); */

    Tsize = strlen(text) - 2;

    if (!file_is_being_sent) {
        sprintf(message, "PST %s %s %d %sT\n", logged_in_UID, active_GID, Tsize, text);
    }
    else {
        fp = fopen(Fname, "rb");
        get_file_data(fp, data);
        Fsize = sizeof(data);

        sprintf(message, "PST %s %s %d %s %s %d %s\n", logged_in_UID, active_GID, Tsize, text, Fname, Fsize, data);

        fclose(fp);
    }

    // communication with server
    /* send_and_receive_TCP(message, reply);
    terminate_string_after_n_tokens(reply, 2);

    sscanf(reply, "%s %s", aux, status);

    validate_post_reply(reply, aux, status); */

    /* DEBUG */
    printf(">> %sT\n", message);
}

void retrieve_command(char* command) {
    // TODO
    return;
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


int validate_logout_command(char* command) {
    
    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 1) {
        fprintf(stderr, "> logout_command: Invalid input.\n");
        return 0;
    }
    return 1;
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


int  validate_unsubscribe_command(char* command, char* GID) {

    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 2) {
        fprintf(stderr, "> validate_unsubscribe_command: Invalid input.\n");
        return 0;
    }
    if (!validate_GID(GID)) {
        fprintf(stderr, "validate_unsubscribe_command: Invalid group ID.\n");
        return 0;
    }
    return 1;
}


int validate_my_groups_command(char* command) {

    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 1) {
        fprintf(stderr, "> validate_my_groups_command: Invalid input.\n");
        return 0;
    }
    return 1;
}


int validate_select_command(char* command, char* GID) {

    int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command != 2) {
        fprintf(stderr, "> validate_select_command: Invalid input.\n");
        return 0;
    }
    if (!validate_GID(GID)) {
        fprintf(stderr, "> validate_select_command: Invalid group ID.\n");
        return 0;
    }
    return 1;
}


int validate_post_command(char* command, char* aux, char* text, char* file_name, int* file_is_being_sent) {

    int i = strlen(aux) + 1;
    int j = 0, k = 0;
    int length = strlen(command) - 1;

    if (command[i++] != '"') {
        /* DEBUG */
        /* printf("+ %s\n", aux);
        printf("+ i = %d\n", i);
        printf("+ command[i] = %c\n", command[i]); */

        fprintf(stderr, "> validate_post_command: Invalid input 1.\n");
        return 0;
    }
    /* DEBUG */
    printf("+ ECHO\n");
    printf("+ command[%d] = %c\n", i, command[i]);

    text[j++] = '"';
    while (command[i] != '"') {
        if (i == length - 1) {
            fprintf(stderr, "> validate_post_command: Invalid input 2.\n");
            return 0;
        }
        /* DEBUG */
        printf("+ command[%d] = %c\n", i, command[i]);

        text[j++] = command[i++];
    }
    text[j] = '"';
    text[j+1] = '\0';

    /* DEBUG */
    printf(">> j = %d\n", j);
    printf(">> length = %d\n", length);

    // 5 to account for the length of "post " + 1 to account for indexing
    if (length > j + 6) {
        /* DEBUG */
        printf("TTTT\n");
        printf(">> j = %d\n", j);
        printf(">> length = %d\n", length);

        *file_is_being_sent = 1;

        i += 2;
        j += 2;
        while (i < length) {
            file_name[k++] = command[i++];
        }
        return 1;
    }
    else {
        /* DEBUG */
        printf("NNNN\n");

        *file_is_being_sent = 0;
        text[++j] = '\0';
        return 1;
    }

    /* int number_of_tokens_command = get_number_of_tokens(command);
    if (number_of_tokens_command == 2) {
        *file_is_being_sent = 0;
        sscanf(command, "%s %s", aux, text);
    }
    else if (number_of_tokens_command == 3) {
        *file_is_being_sent = 1;
        sscanf(command, "%s %s %s", aux, text, file_name);
    }
    else {
        fprintf(stderr, "> validate_post_command: Invalid input.\n");
        return 0;
    }
    return 1; */
}


void validate_register_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);

    /* DEBUG */
    // printf("validate_register_reply: reply:%sT\n", reply);
    // printf("number_of_tokens=%d\n", number_of_tokens_reply);
    // printf("length=%lu\n", strlen(reply));

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


void validate_login_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if ((number_of_tokens_reply != 2) || strcmp("RLO", aux)) {

        /* DEBUG */
        printf("reply:%sT\n", reply);

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
}


void validate_groups_reply(char* reply, char* aux, char* N) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (number_of_tokens_reply < 2) {
        fprintf(stderr, "> validate_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (number_of_tokens_reply == 2 && (strcmp("RGL", aux) || strcmp(N, "0"))) {
        fprintf(stderr, "> validate_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (number_of_tokens_reply > 2 && strcmp("RGL", aux)) {
        fprintf(stderr, "> validate_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
}


void validate_subscribe_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (number_of_tokens_reply > 3 || strcmp(aux, "RGS")) {
        fprintf(stderr, "> validate_subscribe_reply: ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0 && number_of_tokens_reply == 2) {
        printf("> Successfully subscribed to group.\n");
    }
    else if (strcmp(status, "NEW") == 0 && number_of_tokens_reply == 3) {
        printf("> Successfully created new group and subscribed to it.\n");
    }
    else if (strcmp(status, "E_USR") == 0 && number_of_tokens_reply == 2) {
        printf("> Failed to subscribe to group. Invalid user ID.\n");
    }
    else if (strcmp(status, "E_GRP") == 0 && number_of_tokens_reply == 2) {
        printf("> Failed to subscribe to group. Invalid group ID.\n");
    }
    else if (strcmp(status, "E_GNAME") == 0 && number_of_tokens_reply == 2) {
        printf("> Failed to subscribe to group. Invalid group name.\n");
    }
    else if (strcmp(status, "E_FULL") == 0  && number_of_tokens_reply == 2) {
        printf("> Could not create group. Group limit reached.\n");
    }
    else if (strcmp(status, "NOK") == 0 && number_of_tokens_reply == 2) {
        printf("> Failed to subscribe to group.\n");
    }
    else {
        fprintf(stderr, "> ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
}


void validate_usubscribe_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (number_of_tokens_reply != 2 || strcmp(aux, "RGU")) {
        fprintf(stderr, "> validate_unsubscribe_reply: ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0 && number_of_tokens_reply == 2) {
        printf("> Successfully unsubscribed from group.\n");
    }
    else if (strcmp(status, "E_USR") == 0) {
        printf("> Failed to unsubscribe from group. Invalid user ID.\n");
    }
    else if (strcmp(status, "E_GRP") == 0) {
        printf("> Failed to unsubscribe from group. Invalid group ID.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("> Failed to unsubscribe from group.\n");
    }
    else {
        fprintf(stderr, "> ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
}


void validate_my_groups_reply(char* reply, char* aux, char* N) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (number_of_tokens_reply < 2) {
        fprintf(stderr, "> validate_my_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (number_of_tokens_reply == 2 && (strcmp("RGM", aux) || strcmp(N, "0"))) {
        fprintf(stderr, "> validate_my_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(aux, "E_USR") == 0) {
        fprintf(stderr, "> validate_my_groups_reply: Invalid user ID.\n");
        exit(EXIT_FAILURE);
    }

    if (number_of_tokens_reply > 2 && strcmp("RGM", aux)) {
        fprintf(stderr, "> validate_my_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
}


void validate_post_reply(char* reply, char* aux, char* status) {
    // TODO
}


void validate_program_input(int argc, char** argv) {

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
}


void validate_sendto(int n) {

    if(n == -1) {
        perror("ERROR: sendto(): Unable to send message.\n");
        exit(EXIT_FAILURE);
    }
    return;
}


void validate_recvfrom(int n) {

    if(n == -1) {
        perror("ERROR: recvfrom(): Unable to receive message.\n");
        exit(EXIT_FAILURE);
    }
    return;
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


void create_UDP_socket() {

    fd_UDP = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
    if(fd_UDP == -1) {
        perror("ERROR: create_UDP_socket: can't open socket.\n");
        exit(EXIT_FAILURE);
    }
}


void get_address_info_UDP() {

    memset(&hints_UDP, 0, sizeof(hints_UDP)); 
    hints_UDP.ai_family = AF_INET;        // IPv4
    hints_UDP.ai_socktype = SOCK_DGRAM;   // UDP socket

    errcode = getaddrinfo(DSIP, DSport, &hints_UDP, &res_UDP);
    if(errcode != 0) {
        perror("ERROR: get_address_info_UDP\n");
        exit(EXIT_FAILURE);
    }
}


void create_TCP_socket() {

    fd_TCP = socket(AF_INET, SOCK_STREAM, 0); // TCP Socket
    if(fd_TCP == -1) {
        perror("ERROR: create_TCP_socket: can't open socket.\n");
        exit(EXIT_FAILURE);
    }
}


void get_address_info_TCP() {

    memset(&hints_TCP, 0, sizeof(hints_TCP)); 
    hints_TCP.ai_family = AF_INET;        // IPv4
    hints_TCP.ai_socktype = SOCK_STREAM;  // TCP socket

    errcode = getaddrinfo(DSIP, DSport, &hints_TCP, &res_TCP);
    if(errcode != 0) {
        perror("ERROR: get_address_info_TCP\n");
        exit(EXIT_FAILURE);
    }
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
    int i = 0;
    int last_read_character_was_space = 0;

    /* DEBUG */
    // printf(": string:%sT\n", string);
    // printf(": length:%d\n", length);

    for (i = 0; i < length; i++) {

        /* DEBUG */
        // printf(": string[%d]:%c\n", i, string[i]);

        if (i != 0 && !isspace(string[i]) && last_read_character_was_space) {
            /* DEBUG */
            //printf("--> number_of_tokens incremented! CASE 1\n");

            if (isalpha(string[i]) || isdigit(string[i])) {
                ret++;
            }

            /* ret++; */
        }
        if (i == 0 && !isspace(string[i]) /* && (isalpha(string[i] || isdigit(string[i]))) */) {
            /* DEBUG */
            //printf("--> number_of_tokens incremented! CASE 2\n");

            if (isalpha(string[i]) || isdigit(string[i])) {
                ret++;
            }
            /* ret++; */
        }

        if (isspace(string[i])) {
            last_read_character_was_space = 1;
        }
        else {
            last_read_character_was_space = 0;
        }
    }
    return ret;
}


/* int get_file_size(FILE* fp) {


} */


void get_file_data(FILE* fp, char* data) {

    long length;

    if (fp) {

        if (fseek(fp, 0, SEEK_END)) {
            perror("ERROR: fseek\n");
            exit(EXIT_FAILURE);
        }

        length = ftell(fp);
        if (length == -1) {
            perror("ERROR: ftell\n");
            exit(EXIT_FAILURE);
        }

        if(fseek(fp, 0, SEEK_SET)) {
            perror("ERROR: fseek\n");
            exit(EXIT_FAILURE);
        }

        data = malloc(length);
        if (data) {
            if (fread(data, 1, length, fp) == 0) {
                perror("ERROR: fread\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


int get_string_in_quotes(char* command, char* aux, char* text, char* file_name, int* file_is_being_sent) {

    int i = strlen(aux) + 1;
    int j = 0;
    int length = strlen(command);

    if (command[i++] != '"') {
        /* DEBUG */
        /* printf("+ %s\n", aux);
        printf("+ i = %d\n", i);
        printf("+ command[i] = %c\n", command[i]); */

        fprintf(stderr, "> validate_post_command: Invalid input 1.\n");
        return 0;
    }
    /* DEBUG */
    printf("+ ECHO\n");
    printf("+ command[%d] = %c\n", i, command[i]);

    text[j++] = '"';
    while (command[i] != '"') {
        if (i == length - 1) {
            fprintf(stderr, "> validate_post_command: Invalid input 2.\n");
            return 0;
        }
        /* DEBUG */
        printf("+ command[%d] = %c\n", i, command[i]);

        text[j++] = command[i++];
    }
    text[j++] = '"';
    text[j] = '\0';

    if (length > j + 1) {
        *file_is_being_sent = 1;
        i += 2;
        j += 2;
        while (j < length) {
            file_name[j++] = command[i++];
        }
        return 1;
    }
    else {
        *file_is_being_sent = 0;
        return 1;
    }
}


void send_and_receive_UDP(char* message, char* reply){

    int n = sendto(fd_UDP, message, strlen(message), 0, res_UDP->ai_addr, res_UDP->ai_addrlen);
    validate_sendto(n);

    addrlen_UDP = sizeof(addr_UDP);
    n = recvfrom(fd_UDP, reply, MAX_SIZE_REPLY, 0, (struct sockaddr*)&addr_UDP, &addrlen_UDP);
    validate_recvfrom(n);

    return;
}


void send_and_receive_TCP(char* message, char* reply) {
    // TODO
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


/* void show_my_groups(char* reply, char* N_string) {
    // TODO

} */

void clear_string(char* string) {

    if (string != NULL) {
        string[0] = '\0';
    }

    return;
}

/* void terminate_string(char* string) {
    int length = strlen(string);

    if (isspace(string[length-1])) {
        string[length-1] = '\0';
    }
    return;
}  */

void terminate_string_after_n_tokens(char* string, int n) {

    int ret = 0;
    int length = strlen(string);
    int i = 0;
    int last_read_character_was_space = 0;
    int number_of_tokens = get_number_of_tokens(string);

    if (string == NULL) {
        return;
    }
    if (n == 0) {
        string[0] = '\0';
        return;
    }
    if (number_of_tokens < n) {
        fprintf(stderr, "terminate_string_after_n_tokens: n must be smaller than the total number of tokens\n");
        return;
    }

    for (i = 0; i < length; i++) {
        if (i != 0 && !isspace(string[i]) && last_read_character_was_space) {
            ret++;
        }
        if (i == 0 && !isspace(string[i])) {
            ret++;
        }
        if (ret == n + 1) {
            break;
        }

        if (isspace(string[i])) {
            last_read_character_was_space = 1;
        }
        else {
            last_read_character_was_space = 0;
        }
    }
    string[i-1] = '\0';
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
