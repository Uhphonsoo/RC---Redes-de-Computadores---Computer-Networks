#include <stdio.h>
#include <netdb.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "constants.h"
#include "functions.h"
#include "DS_functions.h"

extern int verbose_mode;
extern int Number_of_groups;
extern GROUPLIST *Group_list;

void validate_program_input(int argc, char **argv, char *DSport) {

    if (argc == 1) { // ./DS 
        sprintf(DSport, "%d", PORT_CONST + FENIX_GROUP_NUMBER);
    }
    else if (argc == 2) { // ./DS -v
        sprintf(DSport, "%d", PORT_CONST + FENIX_GROUP_NUMBER);
        verbose_mode = 1;
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-p")) {
            fprintf(stderr, "ERROR: Invalid input. Input has the format ./server [-p DSport] [-v].\n");
            exit(EXIT_FAILURE);
        }
        strcpy(DSport, argv[2]);
    }
    else if (argc == 4) {
        if (strcmp(argv[1], "-p")){
            fprintf(stderr, "ERROR: Invalid input. Input has the format ./server [-p DSport] [-v].\n");
            exit(EXIT_FAILURE);
        }
        if (strcmp(argv[3], "-v")){
            fprintf(stderr, "ERROR: Invalid input. Input has the format ./server [-p DSport] [-v].\n");
            exit(EXIT_FAILURE);
        }

        // ./DS [-p DSport] [-v]
        strcpy(DSport, argv[2]);
        verbose_mode = 1;
    }
    else {
        fprintf(stderr, "ERROR: Invalid input. Input has the format ./server [-p DSport] [-v].\n");
        exit(EXIT_FAILURE);
    }
}


void process_message(char *message, int fd, struct sockaddr_in *addr) {

    char keyword[MAX_SIZE];

    get_first_token(message, keyword);
    if (strcmp(keyword, "REG") == 0) {
        register_command(message, fd, addr);
    }    
    else if (strcmp(keyword, "UNR") == 0) {
        unregister_command(message, fd, addr);
    }
    else if (strcmp(keyword, "LOG") == 0) {
        login_command(message, fd, addr);
    }
    else if (strcmp(keyword, "OUT") == 0) {
        logout_command(message, fd, addr);
    }
    else if (strcmp(keyword, "GLS") == 0) {
        groups_command(message, fd, addr);
    }
    else if (strcmp(keyword, "GSR") == 0) {
        subscribe_command(message, fd, addr);
    }
    else if (strcmp(keyword, "GUR") == 0) {
        unsubscribe_command(message, fd, addr);
    }
    else if (strcmp(keyword, "GLM") == 0) {
        my_groups_command(message, fd, addr);
    }
    else {
        // ERROR
        fprintf(stderr, "ERROR: Invalid input from user.\n");
        char reply[10] = "ERR\n";
        send_reply_UDP(reply, fd, addr);
        exit(EXIT_FAILURE);
    }
}


void process_keyword(char *keyword, int fd, struct sockaddr_in *addr) {

    if (strcmp(keyword, "ULS") == 0) {
        ulist_command(fd, addr);
    }
    else if (strcmp(keyword, "PST") == 0) {
        post_command(fd, addr);
    }
    else if (strcmp(keyword, "RTV") == 0) {
        retrieve_command(fd, addr);
    }
    else {
        // ERROR
        fprintf(stderr, "ERROR: Invalid input from user.\n");
        char reply[10] = "ERR\n";
        send_TCP(reply, fd);
        exit(EXIT_FAILURE);
    }
}


void register_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_register_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, user_ip, user_port);
    }

    clear_string(message);
    free(reply);
}

void unregister_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_unregister_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, user_ip, user_port);
    }

    clear_string(message);
    free(reply);
}


void login_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_login_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, user_ip, user_port);
    }

    clear_string(message);
    free(reply);
}


void logout_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_logout_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user 
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, user_ip, user_port);
    }

    clear_string(message);
    free(reply);
}


void groups_command(char *message, int fd, struct sockaddr_in *addr) {

    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_groups_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        printf("Request with IP %s on port %s.\n", user_ip, user_port);
    }

    clear_string(message);
    free(reply);
}


void subscribe_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_subscribe_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        sscanf(message, "%s %s %s", aux, UID, GID);
        printf("Request by user %s for group %s with IP %s on port %s.\n", UID, GID, user_ip, user_port);
    }

    clear_string(message);
    free(reply);
}


void unsubscribe_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_unsubscribe_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, user_ip, user_port);
    }

    clear_string(message);
    free(reply);
}


void my_groups_command(char *message, int fd, struct sockaddr_in *addr) {

    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_my_groups_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        printf("Request with IP %s on port %s.\n", user_ip, user_port);
    }

    clear_string(message);
    free(reply);
}


void ulist_command(int fd, struct sockaddr_in *addr) {

    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char buffer[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);

    receive_n_chars_TCP(4, buffer, fd);
    process_ulist_message(buffer, reply);

    // communication with server
    send_TCP(reply, fd);

    if (strcmp(reply, "ERR\n") == 0) { // if unexpected protocol message was received from user
        return;
    }

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        printf("Request with IP %s on port %s.\n", user_ip, user_port);
    }

    free(reply);
}


void post_command(int fd, struct sockaddr_in *addr) {

    int  index, Tsize_int, file_is_being_sent = 0;
    char UID[10];
    char GID[10];
    char MID[10];
    char text[241];
    char Tsize[MAX_SIZE];
    char FName[MAX_SIZE];
    char Fsize[MAX_SIZE];
    char buffer[MAX_SIZE];
    char message_dir_path[64];
    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char *)malloc(MAX_REPLY_SIZE);
    
    // receive UID and GID
    receive_n_chars_TCP(10, buffer, fd);
    sscanf(buffer, "%s %s", UID, GID);

    // receive text size
    receive_n_tokens_TCP(1, Tsize, fd);

    Tsize_int = atoi(Tsize);
    // test for presence of file in user message
    file_is_being_sent = receive_n_plus_1_chars_TCP(Tsize_int, text, fd);

    if (!validate_post_message(UID, GID, Tsize, text)) {
        strcpy(reply, "RPT NOK\n");
        send_TCP(reply, fd);
        return;
    }

    // get MID of message to be posted
    index = get_index(Group_list, GID);
    strcpy(MID, Group_list->last_message_available[index]);
    get_next_MID(MID, Group_list, GID);

    // make message directory
    sprintf(message_dir_path, "GROUPS/%s/MSG/%s", GID, MID);
    mkdir(message_dir_path, 0700);

    if (file_is_being_sent) {
        // receive file name, size and data
        receive_n_tokens_TCP(2, buffer, fd);        
        sscanf(buffer, "%s %s", FName, Fsize);
        receive_data_TCP(FName, Fsize, GID, MID, fd);
    }

    make_author_file(UID, GID, MID); // make file for message author
    make_text_file(text, GID, MID);  // make file for message text

    increment_last_message_available(Group_list, GID);

    sprintf(reply, "RPT %s\n", MID);
    send_TCP(reply, fd);

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        printf("Request with IP %s on port %s.\n", user_ip, user_port);
    }

    free(reply);
}


void retrieve_command(int fd, struct sockaddr_in *addr) {

    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    char MID[MAX_SIZE];
    char buffer[MAX_SIZE];
    char user_ip[MAX_SIZE];
    char user_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);

    // receive UID, GID and MID
    receive_n_chars_TCP(14, buffer, fd);
    sscanf(buffer, "%s %s %s", UID, GID, MID);

    if (!validate_retrieve_message(UID, GID, MID)) {
        strcpy(reply, "RRT NOK\n");
        send_TCP(reply, fd);
        return;
    }
    if (!group_has_messages(GID, MID)) {
        strcpy(reply, "RRT EOF\n");
        send_TCP(reply, fd);
        return;
    }

    retrieve_and_send_messages_TCP(UID, GID, MID, fd);

    if (verbose_mode) {
        get_user_ip_and_port(fd, user_ip, user_port, addr);
        printf("Request with IP %s on port %s.\n", user_ip, user_port);
    }

    free(reply);
}


void process_register_message(char *message, char *reply) {

    int  n, number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    
    // validate user input
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    // extract keyword, UID and pass from message
    sscanf(message, "%s %s %s", aux, UID, pass);

    // validate user input
    if (validate_UID(UID) && validate_pass(pass) && !user_is_registered(UID)) {
        if(register_user(UID, pass)) {
            strcpy(reply, "RRG OK\n");
        }
        else {
            strcpy(reply, "RRG NOK\n");
        }
    }
    else if (validate_UID(UID) && validate_pass(pass) && user_is_registered(UID)) {
        strcpy(reply, "RRG DUP\n");
    }
    else {
        strcpy(reply, "RRG NOK\n");
    }
}


void process_unregister_message(char *message, char *reply) {

    int  number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    
    // validate user input
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    // extract keyword, UID and pass from message
    sscanf(message, "%s %s %s", aux, UID, pass);

    // validate user input
    if (validate_UID(UID) && validate_pass(pass)) {
        if(unregister_user(UID, pass)) {
            strcpy(reply, "RUN OK\n");
        }
        else {
            strcpy(reply, "RUN NOK\n");
        }
    }
    else {
        strcpy(reply, "RUN NOK\n");
    }
}


void process_login_message(char *message, char *reply) {

    int  number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];

    // validate user input
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    // extract keyword, UID and pass from message
    sscanf(message, "%s %s %s", aux, UID, pass);

    // validate user input
    if (validate_UID(UID) && validate_pass(pass)) {
        if (login_user(UID, pass)) {
            strcpy(reply, "RLO OK\n");
        }
        else {
            strcpy(reply, "RLO NOK\n");
        }
    }
    else {
        strcpy(reply, "RLO NOK\n");
    }
}


void process_logout_message(char *message, char *reply) {

    int  number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    
    // validate user input
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    // extract keyword, UID and pass from message
    sscanf(message, "%s %s %s", aux, UID, pass);

    // validate user input
    if (validate_UID(UID) && validate_pass(pass)) {
        if (logout_user(UID, pass)) {
            strcpy(reply, "ROU OK\n");
        }
        else {
            strcpy(reply, "ROU NOK\n");
        }
    }
    else {
        strcpy(reply, "ROU NOK\n");
    }
}


void process_groups_message(char *message, char *reply) {

    int  number_of_tokens, number_of_groups;
    char *aux_string;
    
    // validate user input
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 1) {
        strcpy(reply, "ERR\n");
        return;
    }

    // get existing information from file system
    get_groups(Group_list);
    aux_string = GROUPLIST_to_string(Group_list);

    number_of_groups = get_groups(Group_list);
    if (number_of_groups == 0) {
        strcpy(reply, "RGL 0\n");
        return;
    }

    sprintf(reply, "RGL %d ", Group_list->no_groups);
    strcat(reply, aux_string);
    free(aux_string);
}


void process_subscribe_message(char *message, char *reply) {

    int  number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char GName_aux[MAX_SIZE];
    
    // validate user input
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 4) {
        strcpy(reply, "ERR\n");
        return;
    }

    // extract keyword, UID, GID and group name from message
    sscanf(message, "%s %s %s %s", aux, UID, GID, GName);
    get_group_name(GID, GName_aux);

    // validate user input
    if (!validate_UID(UID) || !user_is_registered(UID)) {
        strcpy(reply, "RGS E_USR\n");
        return;
    }
    else if (!validate_GID(GID) || (!group_exists(GID) && strcmp(GID, "00"))) {
        strcpy(reply, "RGS E_GRP\n");
        return;
    }
    else if (!validate_GName(GName) || (strcmp(GName, GName_aux) && strcmp(GID, "00"))) {
        strcpy(reply, "RGS E_GNAME\n");
        return;
    }
    else if (Number_of_groups == 99) {
        strcpy(reply, "RGS E_FULL\n");
        return;
    }

    if (strcmp(GID, "00") == 0) { // if user is creating new group

        create_new_group(GID, GName); // GID gets changed to ID of new group
        if (subscribe_user(UID, GID)) {
            sprintf(reply, "RGS NEW %s\n", GID);
        }
        else {
            strcpy(reply, "RGS NOK\n");
        }
        return;
    }
    else { // else try subscribe user to group
        if (subscribe_user(UID, GID)) {
            strcpy(reply, "RGS OK\n");
        }
        else {
            strcpy(reply, "RGS NOK\n");
        }
        return;
    }
}


void process_unsubscribe_message(char *message, char *reply) {

    int  number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    
    // validate user input
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    // extract keyword, UID and GID
    sscanf(message, "%s %s %s", aux, UID, GID);

    // validate user input
    if (!validate_UID(UID)) {
        strcpy(reply, "RGU E_USR\n");
        return;
    }
    if (!validate_GID(GID)) {
        strcpy(reply, "RGU E_GRP\n");
        return;
    }

    // try unsubscribe user to group
    if(unsubscribe_user(UID, GID)) {
        strcpy(reply, "RGU OK\n");
    }
    else {
        strcpy(reply, "RGU NOK\n");
    }
}


void process_my_groups_message(char *message, char *reply) {

    int  number_of_tokens, number_of_groups;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char *aux_string;
    GROUPLIST *my_groups_list;
    
    // validate user input
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 2) {
        strcpy(reply, "ERR\n");
        return;
    }

    // extract keyword and UID from message
    sscanf(message, "%s %s", aux, UID);

    // validate user input
    if (!validate_UID(UID) || !user_is_logged_in(UID)) {
        strcpy(reply, "RGM E_USR\n");
        return;
    }

    // create GROUPLIST structure for user's groups
    my_groups_list = malloc(sizeof(*my_groups_list));
    initialize_group_list(my_groups_list);

    // get number of groups to which user is subscribed
    number_of_groups = get_my_groups(my_groups_list, UID);
    if (number_of_groups == 0) {
        strcpy(reply, "RGM 0\n");
        return;
    }

    // convert GROUPLIST into a string
    aux_string = GROUPLIST_to_string(my_groups_list);

    sprintf(reply, "RGM %d ", my_groups_list->no_groups);
    strcat(reply, aux_string);
    free(aux_string);
}


void process_ulist_message(char *buffer, char *reply) {

    int  number_of_tokens;
    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char *user_list;
    
    // validate user input
    number_of_tokens = get_number_of_tokens(buffer);
    if (number_of_tokens != 1) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(buffer, "%s", GID);

    // validate user input
    if (!group_exists(GID)) {
        strcpy(reply, "RUL NOK\n");
        return;
    }

    user_list = (char *)malloc(MAX_REPLY_SIZE);
    get_users_of_group(user_list, GID, GName);

    sprintf(reply, "RUL OK %s %s\n", GName, user_list);
}


int validate_post_message(char *UID, char *GID, char *Tsize, char *text) {

    int Tsize_int = atoi(Tsize);

    if (!validate_UID(UID) || !user_is_registered(UID) || !user_is_subscribed_to_group(UID, GID)) {
        return 0;
    }
    if (!validate_GID(GID) || !group_exists(GID)) {
        return 0;
    }
    if (Tsize_int > 240 || strlen(text) > 240) {
        return 0;
    }
    return 1;
}


int validate_retrieve_message(char *UID, char *GID, char *MID) {

    if (!validate_UID(UID) || !validate_GID(GID) || !validate_MID(MID) || !user_is_subscribed_to_group(UID, GID)) {
        return 0;
    }
    return 1;
}


int user_is_registered(char *UID) {

    char file_path[MAX_SIZE];
    FILE *fp;

    sprintf(file_path, "USERS/%s", UID);
    fp = fopen(file_path, "r");

    if (fp == NULL) {
        return 0;
    }
    else {
        return 1;
    }
}


int user_is_logged_in(char *UID) {

    char user_login_path[MAX_SIZE];
    FILE *fp;

    sprintf(user_login_path, "USERS/%s/%s_login.txt", UID, UID);
    fp = fopen(user_login_path, "r");

    if (fp == NULL) {
        return 0;
    }
    else {
        return 1;
    }
}


int user_is_subscribed_to_group(char *UID, char *GID) {

    char file_path[MAX_SIZE];
    FILE *fp;

    sprintf(file_path, "GROUPS/%s/%s.txt", GID, UID);

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        return 0;
    }
    return 1;
}


int register_user(char *UID, char *pass) {

    int ret;
    char user_dir_path[MAX_SIZE];
    char user_pass_path[MAX_SIZE];
    FILE* fp;

    sprintf(user_dir_path, "USERS/%s", UID);

    // create directory for user UID
    ret = mkdir(user_dir_path, 0700);
    if(ret == -1) {
        return 0;
    }

    // create file for user's password
    sprintf(user_pass_path, "USERS/%s/%s_pass.txt", UID, UID);
    fp = fopen(user_pass_path, "a");
    validate_fopen(fp);

    // write user's password to file
    ret = fprintf(fp, "%s", pass);
    validate_fprintf(ret);

    // close file
    ret = fclose(fp);
    validate_fclose(ret);
    
    return 1;
}


int unregister_user(char *UID, char *pass) {
    
    int  ret;
    char user_dir_path[MAX_SIZE];
    char user_pass_path[MAX_SIZE];
    char user_login_path[MAX_SIZE];

    sprintf(user_dir_path, "USERS/%s", UID);
    sprintf(user_pass_path, "USERS/%s/%s_pass.txt", UID, UID);
    sprintf(user_login_path, "USERS/%s/%s_login.txt", UID, UID);

    // check for correct password
    if (!check_password(pass, user_pass_path)) {
        return 0;
    }

    delete_file(user_pass_path);
    delete_file(user_login_path);

    // delete user's directory
    ret = rmdir(user_dir_path);
    if(ret == -1) {
        return 0;
    }
    return 1;
}


int login_user(char *UID, char *pass) {

    char user_dir_path[MAX_SIZE];
    char user_pass_path[MAX_SIZE];
    char user_login_path[MAX_SIZE];

    sprintf(user_dir_path, "USERS/%s", UID);
    sprintf(user_pass_path, "USERS/%s/%s_pass.txt", UID, UID);
    sprintf(user_login_path, "USERS/%s/%s_login.txt", UID, UID);

    // check for correct password
    if (!check_password(pass, user_pass_path)) {
        return 0;
    }
    if (!user_is_registered(UID)) {
        return 0;
    }

    make_file(user_login_path);

    return 1;
}


int logout_user(char *UID, char *pass) {

    int n;
    char user_dir_path[MAX_SIZE];
    char user_pass_path[MAX_SIZE];
    char user_login_path[MAX_SIZE];

    sprintf(user_dir_path, "USERS/%s", UID);
    sprintf(user_pass_path, "USERS/%s/%s_pass.txt", UID, UID);
    sprintf(user_login_path, "USERS/%s/%s_login.txt", UID, UID);

    // check for correct password
    if (!check_password(pass, user_pass_path)) {
        return 0;
    }

    delete_file(user_login_path);

    return 1;
}


int subscribe_user(char *UID, char *GID) {

    char group_user_path[MAX_SIZE];

    // create a new user file under specified group directory
    sprintf(group_user_path, "GROUPS/%s/%s.txt", GID, UID);
    make_file(group_user_path);

    return 1;
}


int unsubscribe_user(char *UID, char *GID) {

    char user_group_path[MAX_SIZE];
    FILE *fp;

    sprintf(user_group_path, "GROUPS/%s/%s.txt", GID, UID);

    fp = fopen(user_group_path, "r");
    if (fp == NULL) {
        return 0;
    }

    // delete user file from specified group directory
    delete_file(user_group_path);
    return 1;
}


int create_new_group(char *GID, char *GName) {

    int  ret, group_number;
    char group_dir_path[MAX_SIZE];
    char group_name_path[MAX_SIZE];
    char group_messages_path[MAX_SIZE];
    FILE *fp;

    // get number of existing groups in file system
    Number_of_groups = get_groups(Group_list);

    if (Number_of_groups < 0) {
        fprintf(stderr, "ERROR: create_new_group: Invalid number of groups in the system.\n");
        return 0;
    }
    else if (Number_of_groups == 99) {
        fprintf(stderr, "ERROR: create_new_group: Group number limit reached.\n");
        return 0;
    }

    group_number = Number_of_groups + 1; // new GID
    if (group_number < 10) {
        sprintf(GID, "0%d", group_number);
    }
    else {
        sprintf(GID, "%d", group_number);
    }

    // create directory for user GID
    sprintf(group_dir_path, "GROUPS/%s", GID);
    ret = mkdir(group_dir_path, 0700);
    if(ret == -1) {
        return 0;
    }

    // create group name file 
    sprintf(group_name_path, "GROUPS/%s/%s_name.txt", GID, GID);
    fp = fopen(group_name_path ,"a");
    validate_fopen(fp);

    ret = fprintf(fp, "%s", GName);
    validate_fprintf(ret);

    ret = fclose(fp);
    validate_fclose(ret);

    // create subdirectory for user messages
    sprintf(group_messages_path, "GROUPS/%s/MSG", GID);
    ret = mkdir(group_messages_path, 0700);
    if(ret == -1) {
        return 0;
    }

    Number_of_groups++;
    return 1;
}


int group_exists(char *GID) {

    char group_path[MAX_SIZE];
    FILE *fp;

    sprintf(group_path, "GROUPS/%s", GID);
    fp = fopen(group_path, "r");

    if (fp == NULL) {
        return 0;
    }
    else {
        fclose(fp);
        return 1;
    }
}


int group_has_messages(char *GID, char *MID) {

    char message_path[40];
    FILE *fp;

    sprintf(message_path, "GROUPS/%s/MSG/%s", GID, MID);

    fp = fopen(message_path, "r");
    if (fp == NULL) {
        return 0;
    }

    return 1;
}


int check_password(char *pass, char *user_pass_path) {

    int Fsize;
    char *read_pass;
    FILE *fp;

    fp = fopen(user_pass_path, "r");
    if (fp == NULL) {
        return 0;
    }

    Fsize = get_file_size(fp);

    read_pass = (char*)malloc(Fsize + 1);
    fread(read_pass, Fsize, 1, fp);
    fclose(fp);

    read_pass[Fsize] = '\0';

    if (strcmp(pass, read_pass)) {
        return 0;
    }
    else {
        return 1;
    }
}


// checks state of file system to get groups' info
int get_groups(GROUPLIST *list) {

    int i = 0;
    char GIDname[MAX_SIZE];
    struct dirent *dir;
    FILE *fp;
    DIR *d;
    
    list->no_groups=0;
    d = opendir("GROUPS");

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.') {
                continue;
            }
            if (strlen(dir->d_name)>2) {
                continue;
            }

            strcpy(list->group_no[i], dir->d_name);
            sprintf(GIDname, "GROUPS/%s/%s_name.txt", dir->d_name, dir->d_name);
            fp = fopen(GIDname, "r");

            update_last_available_message(list, dir->d_name, i);

            if(fp) {
                fscanf(fp, "%24s", list->group_name[i]);
                fclose(fp);
            }
            ++i;
            if(i==99) {
                break;
            }
        }
        list->no_groups = i;
        closedir(d);
    }
    else {
        return(-1);
    }

    if(list->no_groups>1) {
        SortGList(list);
    }

    return(list->no_groups);
}


// checks state of file system to get specified user's groups' info
int get_my_groups(GROUPLIST *list, char *UID) {

    int i = 0;
    char GIDname[MAX_SIZE];
    char group_user_path[MAX_SIZE];
    struct dirent *dir;
    FILE *fp;
    DIR *d;
    
    list->no_groups=0;
    d = opendir("GROUPS");

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.') {
                continue;
            }
            if (strlen(dir->d_name)>2) {
                continue;
            }

            sprintf(group_user_path, "GROUPS/%s/%s.txt", dir->d_name, UID);
            fp = fopen(group_user_path, "r");
            if (fp == NULL) {
                continue;
            }

            strcpy(list->group_no[i], dir->d_name);
            sprintf(GIDname, "GROUPS/%s/%s_name.txt", dir->d_name, dir->d_name);
            fp = fopen(GIDname, "r");

            if(fp) {
                fscanf(fp, "%24s", list->group_name[i]);
                fclose(fp);
            }
            ++i;
            if(i==99) {
                break;
            }
        }
        list->no_groups = i;
        closedir(d);
    }
    else {
        return(-1);
    }

    if(list->no_groups>1) {
        SortGList(list);
    }
    return(list->no_groups);
}


// gets all users of a specified group
int get_users_of_group(char *user_list, char *GID, char *GName) {

    int number_of_subscribed_users = 0;
    char UID[20];
    char group_path[40];
    struct dirent *dir;
    DIR *d;

    if (!group_exists(GID)) {
        return 0;
    }
    get_group_name(GID, GName);

    strcpy(user_list, "");
    sprintf(group_path, "GROUPS/%s", GID);

    d = opendir(group_path);
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[5] == '.') {

            get_UID_from_file_name(UID, dir->d_name);

            strcat(user_list, UID);
            strcat(user_list, " ");
            number_of_subscribed_users++;
        }
    }
    return number_of_subscribed_users;
}


int get_group_name(char *GID, char *GName) {

    int Fsize;
    char group_name_path[MAX_SIZE];
    FILE *fp;

    if (!group_exists(GID)) {
        return 0;
    }

    sprintf(group_name_path, "GROUPS/%s/%s_name.txt", GID, GID);

    fp = fopen(group_name_path, "r");
    Fsize = get_file_size(fp);

    fread(GName, Fsize, 1, fp);

    GName[Fsize] = '\0';

    return 1;
}


void initialize_group_list(GROUPLIST *list) {

    for (int i = 0; i < MAX_GROUPS; i++) {
        list->number_of_messages[i] = 0;
        strcpy(list->last_message_available[i], "0000");
    }
}


// bubble sort for sorting groups in GROUPLIST
void SortGList(GROUPLIST *list) {

    int i, j, n;

    n = list->no_groups;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n - i - 1;  j++) {
            if (strcmp(list->group_no[j], list->group_no[j+1]) > 0) {
                swap_groups(j, j+1, list);
            }
        }
    }
}


void swap_groups(int g1, int g2, GROUPLIST *list) {

    int number_of_messages_aux;
    char group_no_aux[20];
    char group_name_aux[40];
    char last_message_available_aux[5];

    // save first group's values
    number_of_messages_aux = list->number_of_messages[g1];
    strcpy(group_no_aux, list->group_no[g1]);
    strcpy(group_name_aux, list->group_name[g1]);
    strcpy(last_message_available_aux, list->last_message_available[g1]);

    // copy second group's values onto first group's fields
    list->number_of_messages[g1] = list->number_of_messages[g2];
    strcpy(list->group_no[g1], list->group_no[g2]);
    strcpy(list->group_name[g1], list->group_name[g2]);
    strcpy(list->last_message_available[g1], list->last_message_available[g2]);

    // copy first group's values onto second group's fields
    list->number_of_messages[g2] = number_of_messages_aux;
    strcpy(list->group_no[g2], group_no_aux);
    strcpy(list->group_name[g2], group_name_aux);
    strcpy(list->last_message_available[g2], last_message_available_aux);

}


// convert GROUPLIST to string
char *GROUPLIST_to_string(GROUPLIST *list/* , char *reply */) {

    int i = 0;
    char *aux_string = (char *)malloc(MAX_REPLY_SIZE);

    SortGList(list);

    strcpy(aux_string, "");
    for (i = 0; i < list->no_groups; i++) {
        strcat(aux_string, list->group_no[i]);
        strcat(aux_string, " ");
        strcat(aux_string, list->group_name[i]);
        strcat(aux_string, " ");

        strcat(aux_string, list->last_message_available[i]);
        strcat(aux_string, " ");
    }
    return aux_string;
}


// increments specified group's last MID
void increment_last_message_available(GROUPLIST *list, char *GID) {

    int  i, last_message_available_int, new_last_message_available_int;
    char last_message_available[10];
    char new_last_message_available[10];

    i = get_index(list, GID);

    last_message_available_int = atoi(list->last_message_available[i]);
    new_last_message_available_int = last_message_available_int + 1;

    if (new_last_message_available_int > 0 && new_last_message_available_int < 10) {
        sprintf(new_last_message_available, "000%d", new_last_message_available_int);
    }
    else if (new_last_message_available_int > 9 && new_last_message_available_int < 100) {
        sprintf(new_last_message_available, "00%d", new_last_message_available_int);
    }
    else if (new_last_message_available_int > 99 && new_last_message_available_int < 1000) {
        sprintf(new_last_message_available, "0%d", new_last_message_available_int);
    }
    else if (new_last_message_available_int > 999 && new_last_message_available_int < 10000) {
        sprintf(new_last_message_available, "%d", new_last_message_available_int);
    }
    else {
        fprintf(stderr, ">>> ERROR: increment_last_message_available\n");
    }
    strcpy(list->last_message_available[i], new_last_message_available);
}


// returns the MID that comes after the last MID of the provided GID
void get_next_MID(char *MID, GROUPLIST *list, char *GID) {

    int i, last_message_available_int, new_last_message_available_int;
    char last_message_available[10];
    char new_last_message_available[10];

    i = get_index(list, GID);

    last_message_available_int = atoi(list->last_message_available[i]);
    new_last_message_available_int = last_message_available_int + 1;

    if (new_last_message_available_int > 0 && new_last_message_available_int < 10) {
        sprintf(new_last_message_available, "000%d", new_last_message_available_int);
    }
    else if (new_last_message_available_int > 9 && new_last_message_available_int < 100) {
        sprintf(new_last_message_available, "00%d", new_last_message_available_int);
    }
    else if (new_last_message_available_int > 99 && new_last_message_available_int < 1000) {
        sprintf(new_last_message_available, "0%d", new_last_message_available_int);
    }
    else if (new_last_message_available_int > 999 && new_last_message_available_int < 10000) {
        sprintf(new_last_message_available, "%d", new_last_message_available_int);
    }
    else {
        fprintf(stderr, "ERROR: increment_last_message_available\n");
    }
    strcpy(MID, new_last_message_available);
}


// increments value of provided MID
void increment_MID(char *MID) {

    int MID_int, new_MID_int;
    char new_MID[10];

    MID_int = atoi(MID);
    new_MID_int = MID_int + 1;

    if (new_MID_int > 0 && new_MID_int < 10) {
        sprintf(new_MID, "000%d", new_MID_int);
    }
    else if (new_MID_int > 9 && new_MID_int < 100) {
        sprintf(new_MID, "00%d", new_MID_int);
    }
    else if (new_MID_int > 99 && new_MID_int < 1000) {
        sprintf(new_MID, "0%d", new_MID_int);
    }
    else if (new_MID_int > 999 && new_MID_int < 10000) {
        sprintf(new_MID, "%d", new_MID_int);
    }
    else {
        fprintf(stderr, "ERROR: increment_MID\n");
    }
    strcpy(MID, new_MID);
}


// returns number of messages of provided GID
int get_number_of_messages(char *GID, char *MID) {

    int number_of_messages = 0, MID_int = atoi(MID), current_MID = 0;
    char dir_path[MAX_SIZE];
    struct dirent *dir;
    DIR *d;

    sprintf(dir_path, "GROUPS/%s/MSG", GID);
    d = opendir(dir_path);
    while ((dir = readdir(d)) != NULL) {

        if (dir->d_name[0] == '.') {
            continue;
        }

        current_MID = atoi(dir->d_name);
        if (current_MID >= MID_int) {
            number_of_messages++;
        }
    }
    return number_of_messages;
}


// updates the value each group's of last message available based on file system's current state
void update_last_available_message(GROUPLIST *list, char *GID, int i) {

    int current_MID_int = 0, largest_MID_int = 0;
    char largest_MID[10];
    char group_messages_path[64];
    struct dirent *dir;
    DIR *d;
    
    sprintf(group_messages_path, "GROUPS/%s/MSG", GID);
    d = opendir(group_messages_path);
    if (d) {

        while ((dir = readdir(d)) != NULL) {

            if (dir->d_name[0] == '.') {
                continue;
            }
            if (strlen(dir->d_name) != 4) {
                continue;
            }

            current_MID_int = atoi(dir->d_name);
            if (current_MID_int > largest_MID_int) {
                largest_MID_int = current_MID_int;
            }
            list->number_of_messages[i]++;
        }
        closedir(d);

        convert_MID_int_to_string(largest_MID_int, largest_MID);
        strcpy(list->last_message_available[i], largest_MID);
    }
}


// converts MID integer to MID string
void convert_MID_int_to_string(int MID_int, char *MID) {

    if (MID_int >= 0 && MID_int <= 9) {
        sprintf(MID, "000%d", MID_int);
    }
    else if (MID_int >= 10 && MID_int <= 99) {
        sprintf(MID, "00%d", MID_int);
    }
    else if (MID_int >= 100 && MID_int <= 999) {
        sprintf(MID, "0%d", MID_int);
    }
    else if (MID_int >= 1000 && MID_int <= 9999) {
        sprintf(MID, "%d", MID_int);
    }
    else {
        fprintf(stderr, "ERROR: convert_MID_int_to_string\n");
    }
}


// returns index of specified GID in the specified GROUPLIST
int  get_index(GROUPLIST *list, char *GID) {

    int i, n = list->no_groups;

    for (i = 0; i < n; i++) {
        if (strcmp(GID, list->group_no[i]) == 0) {
            return i;
        }
    }
    return -1;
}


// reads a UID from a file name
void get_UID_from_file_name(char *UID, char *file_name) {

    int i = 0;
    for (i = 0; i < 5; i++) {
        UID[i] = file_name[i];
    }
}


// returns requesting user's IP and port
void get_user_ip_and_port(int fd, char *user_ip, char *user_port, struct sockaddr_in *addr) {

    uint16_t port = 0;
    struct in_addr ip_addr = addr->sin_addr;

    inet_ntop(AF_INET, &ip_addr, user_ip, INET_ADDRSTRLEN);
    port = htons(addr->sin_port);
    sprintf(user_port, "%d", port);
}


int create_socket_UDP() {

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    validate_socket(fd);

    return fd;
}


// initializes socket
void get_address_info_UDP(struct addrinfo *hints, struct addrinfo **res, char *port) {

    int ret;

    memset(&(*hints), 0, sizeof((*hints))); 
    (*hints).ai_family = AF_INET;        
    (*hints).ai_socktype = SOCK_DGRAM;   
    (*hints).ai_flags = AI_PASSIVE;

    ret = getaddrinfo(NULL, port, &(*hints), &(*res));
    validate_getaddrinfo(ret);
}


int create_socket_TCP() {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    validate_socket(fd);

    return fd;
}


// initializes socket
void get_address_info_TCP(struct addrinfo *hints, struct addrinfo **res, char *port) {

    int ret;

    memset(&(*hints), 0, sizeof((*hints))); 
    (*hints).ai_family = AF_INET;        
    (*hints).ai_socktype = SOCK_STREAM;  
    (*hints).ai_flags = AI_PASSIVE;

    ret = getaddrinfo(NULL, port, &(*hints), &(*res));
    validate_getaddrinfo(ret);
}


// receives a message from a UDP socket
void receive_message_UDP(int fd, char *message, struct sockaddr_in *addr) {

    int ret;
    socklen_t addrlen ;
    
    addrlen = sizeof(*addr);
    ret = recvfrom(fd, message, MAX_SIZE, 0, (struct sockaddr*)&(*addr), &addrlen);
    validate_recvfrom(ret);

    message[ret] = '\0';
}


// reads n chars from a TCP socket
void receive_n_chars_TCP(int n, char *string, int fd) {

    int ret, bytes_to_read = n * sizeof(char);
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (bytes_to_read > 0) {

        ret = read(fd, ptr, 1);
        validate_read(ret);

        bytes_to_read--;
        ptr++;
    }
    *ptr = '\0';
}


/**
 * reads n chars from a TCP socket
 * returns 1 if char after nth char is ' ', 0 if it is '\n' and -1 otherwise 
 **/
int receive_n_plus_1_chars_TCP(int n, char *string, int fd) {

    int ret = 0, bytes_to_read = (n + 1) * sizeof(char);
    char *ptr;

    string[0] = '\0';

    ptr = string;
    while (bytes_to_read > 0) {

        ret = read(fd, ptr, 1);
        validate_read(ret);

        bytes_to_read--;
        ptr++;
    }

    ptr--;
    if (*ptr == ' ') {
        ret = 1;
    }
    else if (*ptr == '\n') {
        ret = 0;
    }

    *ptr = '\0';
    return ret;
}


// reads n tokens from a TCP socket
void receive_n_tokens_TCP(int n, char *string, int fd) {

    int ret, spaces_to_read = n;
    char *ptr;

    ptr = string;
    while (spaces_to_read > 0) {

        ret = read(fd, ptr, 1);
        validate_read(ret);

        if (isspace(*ptr)) {
            spaces_to_read--;
        }
        ptr++;
    }
    ptr--;
    *ptr = '\0';
}


// reads Fsize bytes of data from TCP socket
void receive_data_TCP(char *FName, char *Fsize, char *GID, char *MID, int fd) {

    int  ret, bytes_to_read = atoi(Fsize);
    char file_path[MAX_SIZE];
    char *buffer[1024];
    FILE *fp;

    sprintf(file_path, "GROUPS/%s/MSG/%s/%s", GID, MID, FName);

    fp = fopen(file_path, "w");
    validate_fopen(fp);

    while (bytes_to_read > 0) {

        ret = read(fd, buffer, 1024);
        validate_read(ret);

        fwrite(buffer, ret, 1, fp);
        bytes_to_read -= ret;
    }
    fclose(fp);
}

// writes n chars into a TCP socket
void send_n_chars_TCP(int n, char *string, int fd) {
    
    int ret, bytes_to_write = n * sizeof(char);

    while (bytes_to_write > 0) {

        ret = write(fd, string, bytes_to_write);
        validate_read(ret);

        bytes_to_write -= ret;
        string += ret;
    }
}


// sends a message into a UDP socket
void send_reply_UDP(char *reply, int fd, struct sockaddr_in *addr) {

    int n;
    socklen_t addrlen;
    
    addrlen = sizeof((*addr));
    n = sendto(fd, reply, strlen(reply), 0, (struct sockaddr*)&(*addr), addrlen);
    validate_sendto(n);
}


// writes a string into a TCP socket
void send_TCP(char *string, int fd) {

    int ret, bytes_to_write = strlen(string) * sizeof(char);
    char *ptr;

    while (bytes_to_write > 0) {
        
        ret = write(fd, string, bytes_to_write);
        validate_write(ret);

        bytes_to_write -= ret;
        string += ret;
    }
}


void retrieve_and_send_messages_TCP(char *UID, char *GID, char *MID, int fd) {

    int  N, Tsize_int, Fsize_int, number_of_files = 0, read_messages = 0;
    char reply[MAX_SIZE];
    char FName[MAX_SIZE];
    char Fsize[MAX_SIZE];
    char Tsize[MAX_SIZE];
    char buffer[MAX_SIZE];
    char text[MAX_TEXT_SIZE];
    char file_path[MAX_SIZE];
    char group_message_path[40];
    char group_messages_path[40];
    char *reply_aux = (char *)malloc(MAX_REPLY_SIZE);
    struct dirent *dir;
    DIR  *d;
    FILE *fp;

    if (!group_exists(GID)) {
        strcpy(reply, "RRT NOK\n");
        send_TCP(reply, fd);
        return;
    }

    sprintf(group_messages_path, "GROUPS/%s/MSG", GID);
    N = get_number_of_messages(GID, MID);

    sprintf(reply_aux, "RRT OK %d", N);
    send_TCP(reply_aux, fd);

    sprintf(group_message_path, "GROUPS/%s/MSG/%s", GID, MID);

    fp = fopen(group_message_path, "r");

    // one iteration per message
    while (fp != NULL && read_messages <= 20) {

        strcpy(buffer, " ");
        send_TCP(buffer, fd);

        number_of_files = get_number_of_files(group_message_path);

        sprintf(file_path, "GROUPS/%s/MSG/%s/T E X T.txt", GID, MID);
        Tsize_int = get_file_size_char(file_path);
        sprintf(Tsize, "%d", Tsize_int);

        // reads text from file
        read_text_from_file(text, file_path, Tsize_int);

        if (number_of_files == 3) { // if there is file associated with message

            d = opendir(group_message_path);
            while ((dir = readdir(d)) != NULL) {

                if (dir->d_name[0] == '.') {
                    continue;
                }
                if (strcmp(dir->d_name, "A U T H O R.txt") == 0) { // if file is author file
                    sprintf(file_path, "GROUPS/%s/MSG/%s/A U T H O R.txt", GID, MID);
                    read_text_from_file(UID, file_path, 5);
                }
                // if file is the message's associated file
                if (strcmp(dir->d_name, "A U T H O R.txt") && strcmp(dir->d_name, "T E X T.txt")) {
                    strcpy(FName, dir->d_name);
                }
            }

            sprintf(file_path, "GROUPS/%s/MSG/%s/%s", GID, MID, FName);
            Fsize_int = get_file_size_char(file_path);
            sprintf(Fsize, "%d", Fsize_int);

            sprintf(reply_aux, "%s %s %s %s / %s %s ", MID, UID, Tsize, text, FName, Fsize);
            
            send_TCP(reply_aux, fd);
            
            // send associeted file's data
            sprintf(file_path, "GROUPS/%s/MSG/%s/%s", GID, MID, FName);
            send_data_TCP(file_path, Fsize, fd);
        }
        else { // if message has no associated file

            d = opendir(group_message_path);
            while ((dir = readdir(d)) != NULL) {

                if (dir->d_name[0] == '.') {
                    continue;
                }
                if (strcmp(dir->d_name, "A U T H O R.txt") == 0) {
                    // read text from author file
                    sprintf(file_path, "GROUPS/%s/MSG/%s/A U T H O R.txt", GID, MID);
                    read_text_from_file(UID, file_path, 5);
                }
            }

            sprintf(reply_aux, "%s %s %s %s", MID, UID, Tsize, text);
            send_TCP(reply_aux, fd);
        }

        read_messages++;
        if (read_messages == 20) { // stop when 20 messages have been read
            break;
        }

        increment_MID(MID); 
        sprintf(group_message_path, "GROUPS/%s/MSG/%s", GID, MID);
        
        fp = fopen(group_message_path, "r");
    }
    strcpy(buffer, " \n");
    send_TCP(buffer, fd);
    free(reply_aux);
}


// opens file and sends its contents into a TCP socket
void send_data_TCP(char *file_path, char *Fsize, int fd) {

    int ret, bytes_to_write, Fsize_int = atoi(Fsize);
    int file_bytes_to_read = Fsize_int;
    /* char *buffer; */
    char buffer[512];
    char *buffer_aux;
    /* char *save_buffer; */
    FILE *fp;

    fp = fopen(file_path, "r");
    validate_fopen(fp);

    while (file_bytes_to_read > 0) { // while end of file is not reached

        /* buffer = (char *)malloc(512); */
        /* save_buffer = buffer; */
        if (file_bytes_to_read > 512) {
            bytes_to_write = 512;
        }
        else {
            bytes_to_write = file_bytes_to_read;
        }

        buffer_aux = buffer;
        fread(buffer_aux, 1, bytes_to_write, fp);

        while (bytes_to_write > 0) {
            ret = write(fd, buffer_aux, bytes_to_write);
            validate_write(ret);

            bytes_to_write -= ret;
            buffer_aux += ret;
        }
        file_bytes_to_read -= bytes_to_write;
        /* free(save_buffer); */
    }
    fclose(fp);
}


// creates a file at specified location
void make_file(char *file_path) {

    FILE *fp;

    fp = fopen(file_path ,"a");
    validate_fopen(fp);
    fclose(fp);
}


// creates an author file and writes author's name into it
void make_author_file(char *UID, char *GID, char *MID) {

    int size = 5 * sizeof(char);
    char author_file_path[MAX_SIZE];
    FILE *fp;

    sprintf(author_file_path, "GROUPS/%s/MSG/%s/A U T H O R.txt", GID, MID);

    fp = fopen(author_file_path, "w");
    validate_fopen(fp);

    fwrite(UID, size, 1, fp);
    fclose(fp);
}


// creates an author file and writes message's text into it
void make_text_file(char *text, char *GID, char *MID) {

    int Tsize = strlen(text) * sizeof(char);
    char text_file_path[MAX_SIZE];
    FILE *fp;

    sprintf(text_file_path, "GROUPS/%s/MSG/%s/T E X T.txt", GID, MID);

    fp = fopen(text_file_path, "w");
    validate_fopen(fp);

    fwrite(text, Tsize, 1, fp);
    fclose(fp);
}


// deletes a file
void delete_file(char *file_path) {

    unlink(file_path);
}


// returns number of files in a directory
int get_number_of_files(char *dir_path) {

    int number_of_files = 0;
    struct dirent *dir;
    DIR *d;

    d = opendir(dir_path);
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] == '.') {
            continue;
        }
        number_of_files++;
    }
    return number_of_files;
}


// reads size chars of text from specified file into string
void read_text_from_file(char *text, char *file_path, int size) {

    FILE *fp;

    fp = fopen(file_path, "r");
    fread(text, size, 1, fp);

    text[size] = '\0';
    fclose(fp);
}
