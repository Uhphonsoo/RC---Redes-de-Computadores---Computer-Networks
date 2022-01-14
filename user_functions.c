#include <stdio.h>
#include <netdb.h> 
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "constants.h"
#include "functions.h"
#include "user_functions.h"

extern int  logged_in;
extern int  fd_UDP, fd_TCP;
extern int  has_active_group;
extern char DSIP[MAX_SIZE];
extern char DSport[MAX_SIZE];
extern char active_GID[MAX_SIZE];
extern char logged_in_UID[MAX_SIZE];
extern char logged_in_pass[MAX_SIZE];
extern socklen_t addrlen_UDP, addrlen_TCP;
extern struct sockaddr_in addr_UDP, addr_TCP;
extern struct addrinfo hints_UDP, *res_UDP, hints_TCP, *res_TCP;

void validate_program_input(int argc, char** argv) {

    // one argument provided
    if (argc == 1) {
        gethostname(DSIP, MAX_SIZE);
        sprintf(DSport, "%d", PORT_CONST + FENIX_GROUP_NUMBER);
    }
    // three arguments provided
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
            print_program_input_error();
    }
    // five arguments provided
    else if (argc == 5) {
        if (strcmp(argv[1], "-n"))
            print_program_input_error();

        if (strcmp(argv[3], "-p"))
            print_program_input_error();

        strcpy(DSIP, argv[2]);
        strcpy(DSport, argv[4]);
    }
    else 
        print_program_input_error();
}


void register_command(char* command) {

    char *reply;
    char *message;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char status[MAX_SIZE];

    // validate command
    sscanf(command, "%s %s %s", aux, UID, pass);
    if(!validate_registration_command(command, UID, pass)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "REG %s %s\n", UID, pass);

    // communication with server
    send_and_receive_UDP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate server reply
    sscanf(reply, "%s %s", aux, status);
    validate_register_reply(reply, aux, status);

    // free memory
    free(message);
    free(reply);
}

void unregister_command(char* command) {

    char *reply;
    char *message;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char status[MAX_SIZE];

    // validate command
    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_registration_command(command, UID, pass)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "UNR %s %s\n", UID, pass);

    // communication with server
    send_and_receive_UDP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate server reply
    sscanf(reply, "%s %s", aux, status);
    validate_unregister_reply(reply, aux, status);

    // free memory
    free(message);
    free(reply);
}

void login_command(char* command) {

    char *reply;
    char *message;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    char status[MAX_SIZE];

    if (logged_in) {
        printf("> Failed to login. A user is already logged in.\n");
        return;
    }

    // validate command
    sscanf(command, "%s %s %s", aux, UID, pass);
    if (!validate_login_command(command, UID, pass)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    strcpy(logged_in_UID, UID);
    strcpy(logged_in_pass, pass);

    sprintf(message, "LOG %s %s\n", UID, pass);

    // communication with server
    send_and_receive_UDP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate server reply
    sscanf(reply, "%s %s", aux, status);
    validate_login_reply(reply, aux, status);

    // free memory
    free(message);
    free(reply);
}

void logout_command(char* command) {

    char *reply;
    char *message;
    char aux[MAX_SIZE];
    char status[MAX_SIZE];

    if (is_empty_string(logged_in_UID) && is_empty_string(logged_in_pass)) {
        printf("> No user is currently logged in.\n");
        return;
    }
    
    // validate command
    sscanf(command, "%s", aux);
    if (!validate_logout_command(command)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "OUT %s %s\n", logged_in_UID, logged_in_pass);

    // communication with server
    send_and_receive_UDP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate server reply
    sscanf(reply, "%s %s", aux, status);
    validate_logout_reply(reply, aux, status);

    // free memory
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

    // validate command
    if (!validate_exit_command(command))
        return;

    if (logged_in) {
        sprintf(command, "logout");
        logout_command(command);
    }

    // free adress info
    freeaddrinfo(res_UDP);
    freeaddrinfo(res_TCP);

    exit(EXIT_SUCCESS);
}

void groups_command(char* command) {

    char *reply;
    char *message;
    char N[MAX_SIZE];
    char aux[MAX_SIZE];

    // validate command
    sscanf(command, "%s", aux);
    if(!validate_groups_command(command))
        return;

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "GLS\n");

    // communication with server
    send_and_receive_UDP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate server reply
    sscanf(reply, "%s %s", aux, N);
    validate_groups_reply(reply, aux, N);

    if (strcmp(N, "0") == 0)
        printf("> There are no available groups.\n");
    else
        show_groups(reply, N);
    
    // free memory
    free(message);
    free(reply);
}


void subscribe_command(char* command) {

    char *reply;
    char *message;
    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char status[MAX_SIZE];

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    // validate command
    sscanf(command, "%s %s %s", aux, GID, GName);
    if(!validate_subscribe_command(command, GID, GName)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "GSR %s %s %s\n", logged_in_UID, GID, GName);

    // communication with server
    send_and_receive_UDP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate reply from server
    sscanf(reply, "%s %s", aux, status);
    validate_subscribe_reply(reply, aux, status);

    // free memory
    free(message);
    free(reply);
}


void unsubscribe_command(char* command) {

    char *reply;
    char *message;
    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    char status[MAX_SIZE];

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    // validate command
    sscanf(command, "%s %s", aux, GID);
    if(!validate_unsubscribe_command(command, GID)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "GUR %s %s\n", logged_in_UID, GID);

    // communication with server
    send_and_receive_UDP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate server reply
    sscanf(reply, "%s %s", aux, status);
    validate_usubscribe_reply(reply, aux, status);

    // free memory
    free(message);
    free(reply);
}


void my_groups_command(char* command) {

    char *reply;
    char *message;
    char N[MAX_SIZE];
    char aux[MAX_SIZE];

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }

    // validate command
    sscanf(command, "%s", aux);
    if(!validate_my_groups_command(command)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "GLM %s\n", logged_in_UID);

    // communication with server
    send_and_receive_UDP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate server reply
    sscanf(reply, "%s %s", aux, N);
    validate_my_groups_reply(reply, aux, N);

    if (strcmp(N, "0") == 0)
        printf("> Currently not subscribed to any group.\n");
    else 
        show_groups(reply, N);

    // free memory
    free(message);
    free(reply);
}


void select_command(char* command) {

    char aux[MAX_SIZE];
    char GID[MAX_SIZE];

    // validate command
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

    char *reply;
    char *message;
    char aux[MAX_SIZE];
    char status[MAX_SIZE];

    if (!has_active_group) {
        printf("> There is no active group.\n");
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "ULS %s\n", active_GID);

    // communication with server
    send_and_receive_TCP(message, reply);

    // check for ERR reply
    if (is_ERR_reply(reply)) {
        free(message);
        free(reply);
        return;
    }

    // validate server reply
    sscanf(reply, "%s %s", aux, status);
    if (validate_ulist_reply(reply, aux, status)) {
        strcat(reply, "\n");
        show_users(reply);
    }

    // free memory
    free(message);
    free(reply);
}


void post_command(char* command) {

    int  ret, file_is_being_sent = 0, Tsize = 0, Fsize = 0;
    char *reply;
    char *message;
    char aux[MAX_SIZE];
    char Fname[MAX_SIZE];
    char status[MAX_SIZE];
    char text[MAX_TEXT_SIZE];
    FILE *fp;

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }
    if (!has_active_group) {
        printf("> There is no active group.\n");
        return;
    }

    // validate command
    sscanf(command, "%s", aux);
    if(!validate_post_command(command, aux, text, Fname, &file_is_being_sent))
        return;

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    strip_quotes_from_string(text);

    Tsize = strlen(text);

    if (!file_is_being_sent) {
        sprintf(message, "PST %s %s %d %s\n", logged_in_UID, active_GID, Tsize, text);

        send_and_receive_TCP(message, reply);
    }
    else {

        create_socket_TCP();
        get_address_info_TCP();

        // CONNECT with server
        ret = connect(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
        validate_connect(ret);

        fp = fopen(Fname, "r");
        validate_fopen(fp);

        Fsize = get_file_size(fp);

        sprintf(message, "PST %s %s %d %s %s %d ", logged_in_UID, active_GID, Tsize, text, Fname, Fsize);

        send_TCP(message);
        send_data_TCP(fp);
        fclose(fp);

        sprintf(message, "\n");
        send_TCP(message);

        receive_TCP(reply);
        close(fd_TCP);
    }

    sscanf(reply, "%s %s", aux, status);
    validate_post_reply(reply, aux, status);

    // free memory
    free(message);
    free(reply);
}


void retrieve_command(char* command) {

    int  n, N_int, Tsize_int, file_came_before = 0;
    char *reply;
    char *message;
    char Tsize[MAX_SIZE];
    char Fsize[MAX_SIZE];
    char Fname[MAX_SIZE];
    char buffer[MAX_SIZE];
    char N[MAX_FILE_SIZE];
    char MID_aux[MAX_SIZE];
    char text[MAX_TEXT_SIZE];
    char aux[10], MID[MAX_SIZE+1], UID[10], status[10];

    if (!logged_in) {
        printf("> No user is currently logged in.\n");
        return;
    }
    if (!has_active_group) {
        printf("> There is no active group.\n");
        return;
    }

    // validate command
    sscanf(command, "%s %s", aux, MID);
    if(!validate_retrieve_command(command, MID)) {
        return;
    }

    message = (char*)malloc(MAX_SIZE);
    reply = (char*)malloc(MAX_REPLY_SIZE);

    sprintf(message, "RTV %s %s %s\n", logged_in_UID, active_GID, MID);

    create_socket_TCP();
    get_address_info_TCP();

    // conect to server
    n = connect(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    validate_connect(n);

    send_TCP(message);

    // receive keyword and status
    receive_n_tokens_TCP(2, reply);

    sscanf(reply, "%s %s", aux, status);
    if (strcmp(status, "NOK") == 0) {
        printf("> Could not retrieve messages.\n");
        return;
    }
    if (strcmp(status, "EOF") == 0) {
        printf("> There are no messages to retrieve.\n");
        return;
    }

    // receive N
    receive_n_tokens_TCP(1, reply);
    sscanf(reply, "%s", N);

    N_int = atoi(N);
    printf("> There are %s messages:\n", N);

    // read first char of MID
    receive_n_chars_TCP(1, buffer);
    buffer[1] = buffer[0]; // this is done in preparation for the string manipulation

    for (int i = 0; i < N_int; i++) {

        if (!file_came_before) {

            // receive MID and UID
            receive_n_chars_TCP(10, reply);
            sscanf(reply, "%s %s", MID_aux, UID);
            
            sprintf(MID, "%c%s", buffer[1], MID_aux);
        }
        else {

            // receive MID and UID
            receive_n_chars_TCP(12, reply);
            sscanf(reply, "%s %s", MID, UID);
        }

        printf("> MID: %s | by UID: %s", MID, UID);

        // receive Tsize
        receive_n_tokens_TCP(1, Tsize);
        
        // receive text
        Tsize_int = atoi(Tsize);
        receive_n_bytes_TCP(Tsize_int, text);

        printf(" | Message: %s\n", text);
        
        // read next two chars to test for file
        receive_n_chars_TCP(2, buffer);

        if (buffer[1] == '/') {  // if there is a file
   
            // receive space
            receive_n_chars_TCP(1, buffer);
            
            // receive Fname and Fsize
            receive_n_tokens_TCP(2, buffer);
            sscanf(buffer, "%s %s", Fname, Fsize);

            printf(">  Associated file: %s\n", Fname);

            // receive data
            receive_data_TCP(Fname, Fsize);

            file_came_before = 1; 
        }
        else {
            file_came_before = 0;
        }
    }

    // free memory
    free(message);
    free(reply);
}


int validate_registration_command(char* command, char* UID, char* pass) {

    if (get_number_of_tokens(command) != 3) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    if (!validate_UID(UID)) {
        print_invalid_UID_message();
        return 0;
    }
    if (!validate_pass(pass)) {
        print_invalid_pass_message();
        return 0;
    }
    return 1;
}


int validate_login_command(char* command, char* UID, char* pass) {

    if (get_number_of_tokens(command) != 3) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    if (!validate_UID(UID)) {
        print_invalid_UID_message();
        return 0;
    }
    if (!validate_pass(pass)) {
        print_invalid_pass_message();
        return 0;
    }
    return 1;
}


int validate_logout_command(char* command) {
    
    if (get_number_of_tokens(command) != 1) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    return 1;
}


int validate_exit_command(char* command) {

    if (get_number_of_tokens(command) != 1) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    return 1;
}


int validate_groups_command(char* command) {

    if (get_number_of_tokens(command) != 1) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    return 1;
}


int validate_subscribe_command(char *command, char *GID, char *GName) {

    if (get_number_of_tokens(command) != 3) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    if (!validate_GID(GID)) {
        print_invalid_GID_message();
        return 0;
    }
    if (!validate_GName(GName)) {
        print_invalid_GName_message();
        return 0;
    }
    return 1;
}


int  validate_unsubscribe_command(char* command, char* GID) {

    if (get_number_of_tokens(command) != 2) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    if (!validate_GID(GID)) {
        print_invalid_GID_message();
        return 0;
    }
    return 1;
}


int validate_my_groups_command(char* command) {

    if (get_number_of_tokens(command) != 1) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    return 1;
}


int validate_select_command(char* command, char* GID) {

    if (get_number_of_tokens(command) != 2) {
        print_invalid_number_of_arguments_message();
        return 0;
    }
    if (!validate_GID(GID)) {
        print_invalid_GID_message();
        return 0;
    }
    return 1;
}


int validate_post_command(char *command, char *aux, char *text, char *Fname, int *file_is_being_sent) {

    int j = 0, k = 0;
    int i = strlen(aux) + 1;
    int length = strlen(command) - 1;

    // check for presence of quotes
    if (command[i] != '"') {
        fprintf(stderr, "> validate_post_command: Invalid input.\n");
        return 0;
    }
    i++;

    text[j++] = '"';
    while (command[i] != '"') {
        if (i == length - 1) {
            fprintf(stderr, "> validate_post_command: Invalid input.\n");
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
        print_invalid_number_of_arguments_message();
        return 0;
    }
    if (!validate_MID(MID)) {
        print_invalid_MID_message();
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
        logged_in = 0;
        clear_string(logged_in_UID);
        clear_string(logged_in_pass);
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
        logged_in = 0;
        clear_string(logged_in_UID);
        clear_string(logged_in_pass);
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
    char new_GID[10];
    
    if (strcmp(aux, "RGS")) {
        fprintf(stderr, "> validate_subscribe_reply: ERROR: Invalid reply from server. REPLY:%s|\n", reply);
        exit(EXIT_FAILURE);
    }

    if (strcmp(status, "OK")  == 0) {
        printf("> Successfully subscribed to group.\n");
    }
    else if (strcmp(status, "NEW") == 0 && number_of_tokens_reply == 3) {
        sscanf(reply, "%s %s %s", aux, status, new_GID);
        printf("> Successfully created group %s and subscribed to it.\n", new_GID);
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


// check whether the reply from server was "ERR\n"
int is_ERR_reply(char *reply) {

    if (strcmp(reply, "ERR\n") == 0) {
        printf("> ERR\n");
        return 1;
    }
    return 0;
}


// create UDP socket
void create_socket_UDP() {

    fd_UDP = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
    validate_socket(fd_UDP);
}


// initialize UDP socket
void get_address_info_UDP() {

    int ret;

    memset(&hints_UDP, 0, sizeof(hints_UDP)); 
    hints_UDP.ai_family = AF_INET;      
    hints_UDP.ai_socktype = SOCK_DGRAM; 

    ret = getaddrinfo(DSIP, DSport, &hints_UDP, &res_UDP);
    validate_getaddrinfo(ret);
}


// create TCP socket
void create_socket_TCP() {

    fd_TCP = socket(AF_INET, SOCK_STREAM, 0);
    validate_socket(fd_TCP);
}


// initialize TCP socket
void get_address_info_TCP() {

    int ret;

    memset(&hints_TCP, 0, sizeof(hints_TCP)); 
    hints_TCP.ai_family = AF_INET;       
    hints_TCP.ai_socktype = SOCK_STREAM; 

    ret = getaddrinfo(DSIP, DSport, &hints_TCP, &res_TCP);
    validate_getaddrinfo(ret);
}


// send UDP message to server and receive its UDP reply
void send_and_receive_UDP(char* message, char* reply){

    int ret;
    struct timeval timeout = {5, 0};
    fd_set current_sockets;

    // socket creation
    create_socket_UDP();
    get_address_info_UDP();

    // send message and validate return value
    ret = sendto(fd_UDP, message, strlen(message), 0, res_UDP->ai_addr, res_UDP->ai_addrlen);
    validate_sendto(ret);

    /* initialize set of file descriptors */
    FD_ZERO(&current_sockets);
    FD_SET(fd_UDP, &current_sockets);

    ret = select(FD_SETSIZE, &current_sockets, NULL, NULL, &timeout);
    validate_select(ret);

    /* Check for requests */
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, &current_sockets)) {
            if (i == fd_UDP) {
                
                // receive reply and validate return value
                addrlen_UDP = sizeof(addr_UDP);
                ret = recvfrom(fd_UDP, reply, MAX_REPLY_SIZE, 0, (struct sockaddr*)&addr_UDP, &addrlen_UDP);
                validate_recvfrom(ret);

                reply[ret] = '\0';
            }
        }
    }
    // close socket
    close(fd_UDP);
}


// send TCP message to server and receive its TCP reply
void send_and_receive_TCP(char* message, char* reply) {

    int  ret;
    int  bytes_to_write = strlen(message) * sizeof(char);
    char *ptr;

    // socket creation
    create_socket_TCP();
    get_address_info_TCP();

    // connect to server
    ret = connect(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    validate_connect(ret);

    while (bytes_to_write > 0) { // while bytes have not all been sent
        
        ret = write(fd_TCP, message, bytes_to_write);
        validate_write(ret);

        bytes_to_write -= ret;
        message += ret;
    }

    // read bytes until '\n' is found
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


// send a TCP message through TCP socket
void send_TCP(char *string) {

    int ret;
    int bytes_to_write = strlen(string) * sizeof(char);

    while (bytes_to_write > 0) { // while bytes have not all been sent

        ret = write(fd_TCP, string, bytes_to_write);
        validate_write(ret);

        bytes_to_write -= ret;
        string += ret;
    }
}


// send file's data through TCP socket
void send_data_TCP(FILE *fp) {

    int  ret;
    int  bytes_to_write;
    char *buffer;
    char *save_buffer;

    while (!feof(fp)) {

        buffer = (char *)malloc(512);
        save_buffer = buffer;
        bytes_to_write = fread(buffer, 1, 512, fp);

        /* buffer[bytes_to_write] = '\0'; */

        while (bytes_to_write > 0) {

            ret = write(fd_TCP, buffer, bytes_to_write);
            validate_write(ret);

            bytes_to_write -= ret;
            buffer += ret;
        }
        free(save_buffer);
    }
}


// receive server's message
void receive_TCP(char *string) {

    int  ret;
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (1) { // read until '\n' is found

        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);

        if (*ptr == '\n') {
            break;
        }

        ptr++;
    }
    *ptr = '\0';
}


// reads n tokens and "kills" the last space with '\0'
void receive_n_tokens_TCP(int n, char *string) {

    int  ret;
    int  spaces_to_read = n;
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


// receives n chars through TCP
void receive_n_chars_TCP(int n, char *string) {

    int  ret;
    int  bytes_to_read = n * sizeof(char);
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (bytes_to_read > 0) {

        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);

        bytes_to_read--;
        ptr++;
    }
    *ptr = '\0';
}


// receives n bytes through TCP
void receive_n_bytes_TCP(int n, char *string) {

    int  ret;
    int  bytes_to_read = n;
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (bytes_to_read > 0) {

        ret = read(fd_TCP, ptr, 1);
        validate_read(ret);

        bytes_to_read--;
        ptr++;
    }
    *ptr = '\0';
}


// receives data and writes it into a file through TCP
void receive_data_TCP(char *FName, char *Fsize) {

    int  ret;
    int  bytes_to_read = atoi(Fsize);
    char file_path[MAX_SIZE];
    char *buffer = (char *)malloc(bytes_to_read);
    FILE *fp;

    sprintf(file_path, "RECEIVED");

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        ret = mkdir(file_path, 0700);
        validate_mkdir(ret);
    }
    else {
        fclose(fp);
    }

    sprintf(file_path, "RECEIVED/%s", FName);

    fp = fopen(file_path, "w");
    validate_fopen(fp);

    while (bytes_to_read > 0) {

        ret = read(fd_TCP, buffer, bytes_to_read);
        validate_read(ret);

        fwrite(buffer, ret, 1, fp);
        bytes_to_read -= ret;   
    }

    fclose(fp);
    // free memory
    free(buffer);
}


void print_program_input_error(){
    fprintf(stderr, "ERROR: Invalid input. Input has the format ./user -n [DSIP] -p [DSport].\n");
    exit(EXIT_FAILURE);
}

void print_invalid_UID_message(){
    fprintf(stderr, "> Invalid user ID.\n");
}

void print_invalid_pass_message(){
    fprintf(stderr, "> Invalid user password.\n");
}

void print_invalid_number_of_arguments_message(){
    fprintf(stderr, "> Invalid number of arguments in input.\n");
}

void print_invalid_GID_message(){
    fprintf(stderr, "> Invalid group ID.\n");
}

void print_invalid_GName_message(){
    fprintf(stderr, "> Invalid group name.\n");
}

void print_invalid_MID_message(){
    fprintf(stderr, "> Invalid message ID.\n");
}