#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <ctype.h>
#include "functions.h"
#include "user_functions.h"
#include "constants.h"

extern int  fd_UDP, fd_TCP;
extern int  errcode;
extern int  logged_in;
extern int  has_active_group;
// extern char Message[MAX_SIZE];
// extern char Reply[MAX_REPLY_SIZE];
extern char message_buffer[MAX_SIZE];
extern char reply_buffer[MAX_REPLY_SIZE];
extern char DSIP[MAX_SIZE];
extern char DSport[MAX_SIZE];
extern char logged_in_UID[MAX_SIZE];
extern char logged_in_pass[MAX_SIZE];
extern char active_GID[MAX_SIZE];
extern char buffer_aux[1024];
extern socklen_t addrlen_UDP, addrlen_TCP;
extern struct addrinfo hints_UDP, *res_UDP, hints_TCP, *res_TCP;
extern struct sockaddr_in addr_UDP, addr_TCP;

void validate_program_input(int argc, char** argv) {

    // ONE argument provided
    if (argc == 1) {
        gethostname(DSIP, MAX_SIZE);
        sprintf(DSport, "%d", PORT_CONST + FENIX_GROUP_NUMBER);
    }
    // THREE arguments provided
    else if (argc == 3){
        if (strcmp(argv[1], "-n") == 0){
            strcpy(DSIP, argv[2]);
            sprintf(DSport, "%d", PORT_CONST + FENIX_GROUP_NUMBER);
        }

        else if (strcmp(argv[1], "-p") == 0){
            strcpy(DSport, argv[2]);
            gethostname(DSIP, MAX_SIZE);
        }

        else 
            input_error();
    }
    // FIVE arguments provided
    else if (argc == 5) {
        if (strcmp(argv[1], "-n"))
            input_error();

        if (strcmp(argv[3], "-p"))
            input_error();

        strcpy(DSIP, argv[2]);
        strcpy(DSport, argv[4]);
    }

    else 
        input_error();
}


void register_command(char* command) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char status[MAX_SIZE];
    char *message;
    char *reply;

    // VALIDATE command
    sscanf(command, "%s %s %s", aux, UID, pass);
    if(!validate_registration_command(command, UID, pass)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "REG %s %s\n", UID, pass);

    // COMMUNICATION with server
    send_and_receive_UDP(message, reply);

    // VALIDATE server reply
    sscanf(reply, "%s %s", aux, status);
    validate_register_reply(reply, aux, status);

    free(message);
    free(reply);
}

void unregister_command(char* command) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char status[MAX_SIZE];
    char *message;
    char *reply;

    // VALIDATE command
    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_registration_command(command, UID, pass)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "UNR %s %s\n", UID, pass);

    // COMMUNICATION with server
    send_and_receive_UDP(message, reply);

    // VALIDATE server reply
    sscanf(reply, "%s %s", aux, status);
    validate_unregister_reply(reply, aux, status);

    free(message);
    free(reply);
}

void login_command(char* command) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char status[MAX_SIZE];
    char *message;
    char *reply;

    if (logged_in) {
        printf("> Failed to login. A user is already logged in.\n");
        return;
    }

    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_login_command(command, UID, pass)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    strcpy(logged_in_UID, UID);
    strcpy(logged_in_pass, pass);

    sprintf(message, "LOG %s %s\n", UID, pass);

    // COMMUNICATION with server
    send_and_receive_UDP(message, reply);

    // VALIDATE server reply
    sscanf(reply, "%s %s", aux, status);
    validate_login_reply(reply, aux, status);

    free(message);
    free(reply);
}

void logout_command(char* command) {

    char aux[MAX_SIZE];
    char status[MAX_SIZE];
    char *message;
    char *reply;

    if (is_empty_string(logged_in_UID) && is_empty_string(logged_in_pass)) {
        printf("> No user is currently logged in.\n");
        return;
    }
    
    // VALIDATE command
    sscanf(command, "%s", aux);
    if (!validate_logout_command(command)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "OUT %s %s\n", logged_in_UID, logged_in_pass);

    // COMMUNICATION with server
    send_and_receive_UDP(message, reply);

    // VALIDATE server reply
    sscanf(reply, "%s %s", aux, status);
    validate_logout_reply(reply, aux, status);

    free(message);
    free(reply);
}

void showuid_command() {

    if (logged_in)
        printf("> User %s is logged in.\n", logged_in_UID);

    else
        printf("> No user is logged in.\n");
}

void exit_command(char* command) {

    char aux[MAX_SIZE];

    sscanf(command, "%s", aux);

    // VALIDATE command
    if (!validate_exit_command(command))
        return;

    // CLOSE & CLEAN
    freeaddrinfo(res_UDP);
    freeaddrinfo(res_TCP);

    exit(EXIT_SUCCESS);
}

void groups_command(char* command) {

    char aux[MAX_SIZE];
    char N[MAX_SIZE];
    char *message;
    char *reply;

    // VALIDATE command
    sscanf(command, "%s", aux);
    if(!validate_groups_command(command)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "GLS\n");

    // COMMUNICATION with server
    send_and_receive_UDP(message, reply);

    // VALIDATE server reply
    sscanf(reply, "%s %s", aux, N);
    validate_groups_reply(reply, aux, N);

    if (strcmp(N, "0") == 0)
        printf("> There are no available groups.\n");
    else
        show_groups(reply, N);
    
    free(message);
    free(reply);
}


void subscribe_command(char* command) {

    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char status[MAX_SIZE];
    char *message;
    char *reply;

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    // VALIDATE command
    sscanf(command, "%s %s %s", aux, GID, GName);
    if(!validate_subscribe_command(command, GID, GName)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "GSR %s %s %s\n", logged_in_UID, GID, GName);

    // COMMUNICATION with server
    send_and_receive_UDP(message, reply);

    // VALIDATE reply from server
    sscanf(reply, "%s %s", aux, status);
    validate_subscribe_reply(reply, aux, status);

    free(message);
    free(reply);
}


void unsubscribe_command(char* command) {

    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    char status[MAX_SIZE];
    char *message;
    char *reply;

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    // VALIDATE command
    sscanf(command, "%s %s", aux, GID);
    if(!validate_unsubscribe_command(command, GID)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "GUR %s %s\n", logged_in_UID, GID);

    // COMMUNICATION with server
    send_and_receive_UDP(message, reply);

    // VALIDATE server reply
    sscanf(reply, "%s %s", aux, status);
    validate_usubscribe_reply(reply, aux, status);

    free(message);
    free(reply);
}


void my_groups_command(char* command) {

    char aux[MAX_SIZE];
    char N[MAX_SIZE];
    char *message;
    char *reply;

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    // VALIDATE command
    sscanf(command, "%s", aux);
    if(!validate_my_groups_command(command)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "GLM %s\n", logged_in_UID);

    /* DEBUG */
    printf(">>> message = %s|\n", message);

    // COMMUNICATION with server
    send_and_receive_UDP(message, reply);

    /* DEBUG */
    printf(">>> reply = %s|\n", reply);

    // VALIDATE server reply
    sscanf(reply, "%s %s", aux, N);
    validate_my_groups_reply(reply, aux, N);

    if (strcmp(N, "0") == 0)
        printf("> Currently not subscribed to any group.\n");
    else 
        show_groups(reply, N);

    free(message);
    free(reply);
}


void select_command(char* command) {

    char aux[MAX_SIZE];
    char GID[MAX_SIZE];

    // VALIDATE command
    sscanf(command, "%s %s", aux, GID);
    if(!validate_select_command(command, GID)) {
        return;
    }

    // SET active group
    strcpy(active_GID, GID);
    has_active_group = 1;

    printf("> Group %s selected.\n", active_GID);
}


void showgid_command() {

    if (has_active_group)
        printf("> Group %s is selected.\n", active_GID);
    else 
        printf("> No group is selected.\n");
}


void ulist_command() {

    char aux[MAX_SIZE];
    char N[MAX_SIZE];
    char status[MAX_SIZE];
    char *message;
    char *reply;

    if (!has_active_group) {
        printf("> There is no active group.\n");
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "ULS %s\n", active_GID);

    /* DEBUG */
    printf(">>> ulist: message = %s|\n", message);

    // COMMUNICATION with server
    send_and_receive_TCP(message, reply, strlen(message));

    /* DEBUG */
    printf(">>> ulist: reply = %s|\n", reply);

    // VALIDATE server reply
    sscanf(reply, "%s %s", aux, status);
    if (validate_ulist_reply(reply, aux, status))
        show_users(reply);

    free(message);
    free(reply);
}


void post_command(char* command) {

    int n, ret, read_bytes = 0;
    int file_is_being_sent = 0;
    int Tsize = 0;
    int Fsize = 0;
    char aux[MAX_SIZE];
    char text[MAX_TEXT_SIZE];
    char Fname[MAX_SIZE];
    char *ptr;
    char status[MAX_SIZE];
    char *message;
    char *reply;
    FILE *fp;

    /* DEBUG */
    login_command("login 77777 password\n");
    select_command("select 01\n");

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }
    if (!has_active_group) {
        printf("> There is no active group.\n");
        return;
    }

    // VALIDATE command
    sscanf(command, "%s", aux);
    if(!validate_post_command(command, aux, text, Fname, &file_is_being_sent))
        return;

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    Tsize = strlen(text);

    if (!file_is_being_sent) {
        sprintf(message, "PST %s %s %d %s\n", logged_in_UID, active_GID, Tsize, text);

        send_and_receive_TCP(message, reply, strlen(message));
    }
    else {

        create_TCP_socket();
        get_address_info_TCP();

        n = connect(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
        validate_connect(n);

        fp = fopen(Fname, "r");
        validate_fopen(fp);

        Fsize = get_file_size(fp);

        sprintf(message, "PST %s %s %d %s %s %d ", logged_in_UID, active_GID, Tsize, text, Fname, Fsize);

        send_TCP(message);
        send_data_TCP(fp, Fsize);
        fclose(fp);

        sprintf(message, "\n");
        send_TCP(message);

        receive_TCP(reply);
        close(fd_TCP);
    }

    sscanf(reply, "%s %s", aux, status);
    validate_post_reply(reply, aux, status);

    free(message);
    free(reply);
}


void retrieve_command(char* command) {

    int n;
    int write_bytes_left;
    int file_is_being_sent = 0;
    int read_bytes = 0;
    int N_int;
    int Tsize_int;
    char buffer[MAX_SIZE];
    char aux[10];
    char MID[10];
    char UID[10];
    char Tsize[MAX_SIZE];
    char text[MAX_TEXT_SIZE];
    char Fsize[MAX_SIZE];
    char Fname[MAX_SIZE];
    char *data;
    char status[10];
    char N[MAX_FILE_SIZE];
    char *message;
    char *reply;
    char *ptr;
    FILE *fp; 

    /* DEBUG */
    login_command("login 72182 hhhhhhhh\n");
    select_command("select 72\n");
    /* login_command("login 77777 hhhhhhhh\n"); */
    /* login_command("login 77777 password\n"); */
    /* select_command("select 06\n"); */


    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }
    if (!has_active_group) {
        printf("> There is no active group.\n");
        return;
    }

    // VALIDATE command
    sscanf(command, "%s %s", aux, MID);
    if(!validate_retrieve_command(command, MID)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "RTV %s %s %s\n", logged_in_UID, active_GID, MID);

    /* DEBUG */
    /* printf("message = %s|\n", message); */

    /* vvv */

    create_TCP_socket();
    get_address_info_TCP();

    n = connect(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    validate_connect(n);

    write_bytes_left = strlen(message);
    while (write_bytes_left > 0) {
        n = write(fd_TCP, message, strlen(message));
        validate_write(n);
        write_bytes_left -= n;
        message += n;
    }

    // TODO !!! the receive part
    receive_first_tokens_retrieve_TCP(reply);

    /* DEBUG */
    /* printf(">>> retrieve: reply = %s|\n", reply); */

    sscanf(reply, "%s %s", aux, status);
    if (strcmp(status, "NOK") == 0) {
        printf("> Could not retrieve. There was a problem with the retrieve request.\n");
        return;
    }
    if (strcmp(status, "EOF") == 0) {
        printf("> There are no messages to retrieve.\n");
        return;
    }

    /* DEBUG */
    /* printf(">>> aux = %s|\n", aux);
    printf(">>> status = %s|\n", status); */

    receive_n_tokens_TCP(1, reply);
    sscanf(reply, "%s", N);

    N_int = atoi(N);

    for (int i = 0; i < N_int; i++) {

        receive_n_chars_TCP(11, reply);
        sscanf(reply, "%s %s", MID, UID);

        /* DEBUG */
        printf(">>> MID = %s|\n", MID);
        printf(">>> UID = %s|\n", UID);

        // space
        receive_n_tokens_TCP(1, Tsize);
        
        // receive text
        Tsize_int = atoi(Tsize);
        receive_n_bytes_TCP(Tsize_int, text);

        /* DEBUG */
        printf(">>> text = %s|\n", text);

        // read next char to test for file
        receive_n_chars_TCP(1, buffer);

        // if there is a file
        if (buffer[0] == ' ') {

            receive_n_chars_TCP(2, buffer);

            receive_n_tokens_TCP(2, buffer);
            sscanf(buffer, "%s %s", Fname, Fsize);

            receive_data_TCP(Fname, Fsize);
        }

        /* DEBUG */
        printf("---------------------\n");
    }

    /* ^^^ */

    // COMMUNICATION with server
    /* send_and_receive_TCP(message, reply, strlen(message)); */

    // VALIDATE server reply
    /* sscanf(reply, "%s %s, %s", aux, status, N);
    if (validate_retrieve_reply(reply, aux, status, N))
        show_messages(reply); */

    free(message);
    free(reply);
}

// Validation functions

int validate_registration_command(char* command, char* UID, char* pass) {

    if (get_number_of_tokens(command) != 3) {
        invalid_NArgs();
        return 0;
    }
    if (!validate_UID(UID)) {
        invalid_UID();
        return 0;
    }
    if (!validate_pass(pass)) {
        invalid_Upass();
        return 0;
    }
    return 1;
}


int validate_login_command(char* command, char* UID, char* pass) {

    if (get_number_of_tokens(command) != 3) {
        invalid_NArgs();
        return 0;
    }
    if (!validate_UID(UID)) {
        invalid_UID();
        return 0;
    }
    if (!validate_pass(pass)) {
        invalid_Upass();
        return 0;
    }
    return 1;
}


int validate_logout_command(char* command) {
    
    if (get_number_of_tokens(command) != 1) {
        invalid_NArgs();
        return 0;
    }
    return 1;
}


int validate_exit_command(char* command) {

    if (get_number_of_tokens(command) != 1) {
        invalid_NArgs();
        return 0;
    }
    return 1;
}


int validate_groups_command(char* command) {

    if (get_number_of_tokens(command) != 1) {
        invalid_NArgs();
        return 0;
    }
    return 1;
}


int validate_subscribe_command(char* command, char* GID, char* GName) {

    if (get_number_of_tokens(command) != 3) {
        invalid_NArgs();
        return 0;
    }
    if (!validate_GID(GID)) {
        invalid_GID();
        return 0;
    }
    if (!validate_GName(GName)) {
        invalid_GName();
        return 0;
    }
    return 1;
}


int  validate_unsubscribe_command(char* command, char* GID) {

    if (get_number_of_tokens(command) != 2) {
        invalid_NArgs();
        return 0;
    }
    if (!validate_GID(GID)) {
        invalid_GID();
        return 0;
    }
    return 1;
}


int validate_my_groups_command(char* command) {

    if (get_number_of_tokens(command) != 1) {
        invalid_NArgs();
        return 0;
    }
    return 1;
}


int validate_select_command(char* command, char* GID) {

    if (get_number_of_tokens(command) != 2) {
        invalid_NArgs();
        return 0;
    }
    if (!validate_GID(GID)) {
        invalid_GID();
        return 0;
    }
    return 1;
}


int validate_post_command(char* command, char* aux, char* text, char* Fname, int* file_is_being_sent) {

    int i = strlen(aux) + 1;
    int j = 0, k = 0;
    int length = strlen(command) - 1;

    if (command[i] != '"') {
        fprintf(stderr, "> validate_post_command: Invalid input 1.\n");
        return 0;
    }
    i++;

    text[j++] = '"';
    while (command[i] != '"') {
        if (i == length - 1) {
            fprintf(stderr, "> validate_post_command: Invalid input 2.\n");
            return 0;
        }
        text[j++] = command[i++];
    }
    text[j] = '"';
    text[j+1] = '\0';

    // 5 to account for the length of "post " + 1 to account for indexing
    // j is the length of text
    if (length > j + 6) {
        *file_is_being_sent = 1;

        i += 2;
        j += 2;

        while (i < length) {
            Fname[k++] = command[i++];
        }
        Fname[k] = '\0';
    }
    else {
        *file_is_being_sent = 0;
        text[++j] = '\0';
    }
    return 1;
}


int validate_retrieve_command(char *command, char *MID) {

    if (get_number_of_tokens(command) != 2) {
        invalid_NArgs();
        return 0;
    }
    if (!validate_MID(MID)) {
        invalid_MID();
        return 0;
    }
    return 1;
}

void validate_register_reply(char *reply, char *aux, char *status) {

    if ((get_number_of_tokens(reply) != 2) || strcmp("RRG", aux)) {
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
        fprintf(stderr, "login_command(): Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK") == 0) {
        logged_in = 1;
        printf("> User successfully logged in.\n");
    }
    else if (strcmp(status, "NOK") == 0) {
        printf("> Failed to login.\n");
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
    /* if (number_of_tokens_reply > 3 || strcmp(aux, "RGS")) {
        fprintf(stderr, "> validate_subscribe_reply: ERROR: Invalid reply from server. REPLY:%s|\n", reply);
        exit(EXIT_FAILURE);
    } */

    if (strcmp(status, "OK")  == 0 /* && number_of_tokens_reply == 2 */) {
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
        printf("> Could not create and subscribe to group. Group limit reached.\n");
    }
    else if (strcmp(status, "NOK") == 0 && number_of_tokens_reply == 2) {
        printf("> Failed to subscribe to group.\n");
    }
    else {
        fprintf(stderr, "> ERROR: Invalid reply from server. REPLY:%s|\n", reply);
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
    if (strcmp(aux, "RGM")) {
        fprintf(stderr, "> validate_my_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (number_of_tokens_reply < 2) {
        fprintf(stderr, "> validate_my_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (number_of_tokens_reply == 2 && strcmp(N, "0")) {
        fprintf(stderr, "> validate_my_groups_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(aux, "E_USR") == 0) {
        fprintf(stderr, "> validate_my_groups_reply: Invalid user ID.\n");
        exit(EXIT_FAILURE);
    }
}


int validate_ulist_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (strcmp(aux, "RUL")) {
        fprintf(stderr, "> validate_ulist_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (number_of_tokens_reply < 2) {
        fprintf(stderr, "> validate_ulist_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (number_of_tokens_reply == 2 && strcmp(status, "NOK")) {
        fprintf(stderr, "> validate_ulist_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(aux, "NOK") == 0) {
        fprintf(stderr, "> validate_ulist_reply: The group does not exist.\n");
        return 0;
    }
    return 1;
}


void validate_post_reply(char* reply, char* aux, char* status) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (number_of_tokens_reply != 2 || strcmp(aux, "RPT")) {
        fprintf(stderr, "> validate_post_reply: ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "NOK") == 0) {
        printf("> Failed to post message to group.\n");
    }
    else if (validate_MID(status)) {
        printf("> Successfully posted message %s.\n", status);
    }
    else {
        fprintf(stderr, "> ERROR: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
}


int validate_retrieve_reply(char* reply, char* aux, char* status, char* N) {

    int number_of_tokens_reply = get_number_of_tokens(reply);
    if (strcmp(aux, "RRT")) {
        fprintf(stderr, "> validate_retrieve_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(status, "NOK") == 0) {
        printf("> Failed to retrieve messages.\n");
        return 0;
    }
    if (number_of_tokens_reply < 2) {
        fprintf(stderr, "> validate_retrieve_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    /* if (number_of_tokens_reply == 3 && strcmp(N, "0")) {
        fprintf(stderr, "> validate_retrieve_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    } */
    if (number_of_tokens_reply > 3 && strcmp(N, "0") == 0) {
        fprintf(stderr, "> validate_retrieve_reply: Invalid reply from server.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(status, "EOF") == 0) {
        printf("> No new messages are available.\n");
        return 0;
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

void send_and_receive_UDP(char* message, char* reply){

    int n;

    // SOCKET creation
    create_UDP_socket();
    get_address_info_UDP();

    // SEND message and validate return value
    n = sendto(fd_UDP, message, strlen(message), 0, res_UDP->ai_addr, res_UDP->ai_addrlen);
    validate_sendto(n);

    // RECEIVE reply and validate return value
    addrlen_UDP = sizeof(addr_UDP);
    n = recvfrom(fd_UDP, reply, MAX_REPLY_SIZE, 0, (struct sockaddr*)&addr_UDP, &addrlen_UDP);
    validate_recvfrom(n);

    // CLOSE socket
    close(fd_UDP);
}


void send_and_receive_TCP(char* message, char* reply, int write_n) {

    int ret;
    int bytes_to_write = strlen(message) * sizeof(char);
    char *ptr;

    create_TCP_socket();
    get_address_info_TCP();

    int n = connect(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    validate_connect(n);

    while (bytes_to_write > 0) {
        
        ret = write(fd_TCP, message, bytes_to_write);
        validate_write(ret);

        bytes_to_write -= ret;
        message += ret;
    }

    ptr = reply;
    while (1) {

        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);
        
        if (*ptr == '\n') {
            break;
        }
        
        ptr++;
    }
    *ptr = '\0';
    close(fd_TCP);
}


void send_TCP(char *string) {

    int ret;
    int bytes_to_write = strlen(string) * sizeof(char);
    char *ptr;

    while (bytes_to_write > 0) {

        ret = write(fd_TCP, string, bytes_to_write);
        validate_write(ret);

        bytes_to_write -= ret;
        string += ret;
    }
}


void send_data_TCP(FILE *fp, int Fsize) {

    int ret;
    int bytes_to_write = Fsize;
    char *buffer = (char *)malloc(Fsize);

    fread(buffer, Fsize, 1, fp);
    while (bytes_to_write > 0) {

        ret = write(fd_TCP, buffer, bytes_to_write);
        validate_write(ret);

        bytes_to_write -= ret;
        buffer += ret;
    }
}


void receive_TCP(char *string) {

    int ret;
    int i = 0;
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (1) {

        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);

        if (*ptr == '\n') {
            break;
        }

        ptr++;
    }
    *ptr = '\0';
}


void receive_first_tokens_retrieve_TCP(char *string) {

    // int ret;
    // int bytes_to_read = n * sizeof(char);
    // int read_bytes = 0;
    int ret;
    int spaces_to_read = 2;
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (spaces_to_read > 0) {
        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);

        if (isspace(*ptr)) {
            spaces_to_read--;
        }
        ptr++;
    }
    *ptr = '\0';
}


void receive_n_tokens_TCP(int n, char *string) {

    int ret;
    int spaces_to_read = n;
    char *ptr;
    
    string[0] = '\0';

    ptr = string;
    while (spaces_to_read > 0) {
        
        // (1 char = 1 Byte)
        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);

        if (isspace(*ptr)) {
            spaces_to_read--;
        }
        ptr++;
    }
    ptr--;
    *ptr = '\0';
}


void receive_n_chars_TCP(int n, char *string) {

    int ret;
    int bytes_to_read = n * sizeof(char);
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (bytes_to_read > 0) {

        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);

        bytes_to_read--;
        ptr++;
    }
}

void receive_n_bytes_TCP(int n, char *string) {

    int ret;
    int bytes_to_read = n;
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (bytes_to_read > 0) {

        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);

        bytes_to_read--;
        ptr++;
    }
}


void receive_data_TCP(char *file_path, char *Fsize) {

    int ret;
    int Fsize_int = atoi(Fsize);
    int bytes_to_read = Fsize_int;
    char buffer[1024];
    FILE *fp;

    fp = fopen(file_path, "w");
    validate_fopen(fp);

    while(bytes_to_read > 0) {

        ret = read(fd_TCP, buffer, 1024);
        validate_read(ret);

        fwrite(buffer, ret, 1, fp);
        
        bytes_to_read -= ret;   
    }
    fclose(fp);
}


void input_error(){
    fprintf(stderr, "ERROR: Invalid input. Input has the format ./user -n [DSIP] -p [DSport].\n");
    exit(EXIT_FAILURE);
}

void invalid_UID(){
    fprintf(stderr, "> Invalid user ID.\n");
}

void invalid_Upass(){
    fprintf(stderr, "> Invalid user password.\n");
}

void invalid_NArgs(){
    fprintf(stderr, "> Invalid number of arguments in input.\n");
}

void invalid_GID(){
    fprintf(stderr, "Invalid group ID.\n");
}

void invalid_GName(){
    fprintf(stderr, "Invalid group name.\n");
}

void invalid_MID(){
    fprintf(stderr, "Invalid message ID.\n");
}