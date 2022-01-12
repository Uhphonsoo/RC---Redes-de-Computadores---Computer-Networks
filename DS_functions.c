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
#include <arpa/inet.h>
#include "constants.h"
#include "functions.h"
#include "DS_functions.h"

extern int  verbose_mode;
extern int Number_of_groups;
extern GROUPLIST *Group_list;

void validate_program_input(int argc, char **argv, char *DSport) {

    if (argc == 1) {
        sprintf(DSport, "%d", PORT_CONST + FENIX_GROUP_NUMBER);
    }
    else if (argc == 2) {
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
        clear_string(message);
    }
    else if (strcmp(keyword, "UNR") == 0) {

        unregister_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "LOG") == 0) {

        login_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "OUT") == 0) {

        logout_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "GLS") == 0) {

        groups_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "GSR") == 0) {

        subscribe_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "GUR") == 0) {

        unsubscribe_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "GLM") == 0) {

        my_groups_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "ULS") == 0) {

        ulist_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "PST") == 0) {

        post_command(message, fd, addr);
        clear_string(message);
    }
    else if (strcmp(keyword, "RTV") == 0) {

        retrieve_command(message, fd, addr);
        clear_string(message);
    }
    else {
        // ERR
        fprintf(stderr, "ERROR: process_message\n");
    }
}


void process_keyword(char *keyword, int fd, struct sockaddr_in *addr) {

    if (strcmp(keyword, "ULS") == 0) {

        ulist_command(keyword, fd, addr);
        clear_string(keyword);
    }
    else if (strcmp(keyword, "PST") == 0) {

        post_command(keyword, fd, addr);
        clear_string(keyword);
    }
    else if (strcmp(keyword, "RTV") == 0) {

        retrieve_command(keyword, fd, addr);
        clear_string(keyword);
    }
    else {
        // ERR
        fprintf(stderr, "ERROR: process_keyword\n");
    }
}


void register_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    // char status[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_register_message(message, reply);

    /* DEBUG */
    /* printf(">>> reply = %s|\n", reply); */

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, client_ip, client_port);
    }

    clear_string(message);
    free(reply);
}

void unregister_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_unregister_message(message, reply);

    /* DEBUG */
    /* printf(">>> reply = %s|\n", reply); */

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, client_ip, client_port);
    }

    clear_string(message);
    free(reply);
}


void login_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    // char status[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_login_message(message, reply);

    /* DEBUG */
    /* printf(">>> reply = %s|\n", reply); */

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, client_ip, client_port);
    }

    clear_string(message);
    free(reply);
}


void logout_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    // char status[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_logout_message(message, reply);

    /* DEBUG */
    /* printf(">>> reply = %s|\n", reply); */

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, client_ip, client_port);
    }

    clear_string(message);
    free(reply);
}


void groups_command(char *message, int fd, struct sockaddr_in *addr) {

    // char aux[MAX_SIZE];
    // char UID[MAX_SIZE];
    // char status[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);

    /* DEBUG */
    printf("--- groups_command called\n");
    
    process_groups_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        printf("Request with IP %s on port %s.\n", client_ip, client_port);
    }

    clear_string(message);
    free(reply);
}


void subscribe_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    // char status[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    terminate_string_after_n_tokens(message, 4);
    process_subscribe_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        sscanf(message, "%s %s %s", aux, UID, GID);
        printf("Request by user %s for group %s with IP %s on port %s.\n", UID, GID, client_ip, client_port);
    }

    clear_string(message);
    free(reply);
}


void unsubscribe_command(char *message, int fd, struct sockaddr_in *addr) {

    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_unsubscribe_message(message, reply);

    /* DEBUG */
    /* printf(">>> reply = %s|\n", reply); */

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        sscanf(message, "%s %s", aux, UID);
        printf("Request by user %s with IP %s on port %s.\n", UID, client_ip, client_port);
    }

    clear_string(message);
    free(reply);
}


void my_groups_command(char *message, int fd, struct sockaddr_in *addr) {

    // char aux[MAX_SIZE];
    // char UID[MAX_SIZE];
    // char status[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    
    process_my_groups_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        printf("Request with IP %s on port %s.\n", client_ip, client_port);
    }

    clear_string(message);
    free(reply);
}


void ulist_command(char *keyword, int fd, struct sockaddr_in *addr) {

    // char aux[MAX_SIZE];
    // char UID[MAX_SIZE];
    // char status[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    char message_remainder[MAX_SIZE];

    receive_n_chars_TCP(4, message_remainder, fd);
    process_ulist_message(message_remainder, reply);

    // communication with server
    send_TCP(reply, fd);

    if (strcmp(reply, "ERR\n") == 0) {
        return;
    }

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        printf("Request with IP %s on port %s.\n", client_ip, client_port);
    }

    clear_string(message_remainder);
    free(reply);
}


void post_command(char *keyword, int fd, struct sockaddr_in *addr) {

    int i;
    int number_of_tokens;
    int file_is_being_sent = 0;
    int Tsize_int;
    char UID[10];
    char GID[10];
    char Tsize[MAX_SIZE];
    char FName[MAX_SIZE];
    char Fsize[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char text[241];
    char MID[10];
    char buffer[MAX_SIZE];
    char message_dir_path[64];
    char *reply = (char *)malloc(MAX_REPLY_SIZE);
 
    /* vvv */
    receive_n_chars_TCP(10, buffer, fd);

    sscanf(buffer, "%s %s", UID, GID);

    /* DEBUG */
    printf(">>> UID = %s| GID = %s|\n", UID, GID);

    receive_n_tokens_TCP(1, Tsize, fd);

    /* DEBUG */
    printf(">>> Tsize = %s|\n", Tsize);

    Tsize_int = atoi(Tsize);
    file_is_being_sent = receive_n_plus_1_chars_TCP(Tsize_int, text, fd);

    if (!validate_post_message(UID, GID)) {
        strcpy(reply, "RPT NOK\n");
        send_TCP(reply, fd);
        return;
    }

    /* DEBUG */
    // printf(">>> UID = %s|\n", UID);
    // printf(">>> GID = %s|\n", GID);
    // printf(">>> Tsize = %s|\n", Tsize);
    /* printf(">>> text = %s|\n", text);
    printf(">>> file_is_being_sent = %d\n", file_is_being_sent); */

    int index = get_index(Group_list, GID);
    strcpy(MID, Group_list->last_message_available[index]);

    /* DEBUG */
    /* printf(">>> Group_list->last_message_available[index] = %s|\n", Group_list->last_message_available[index]); */

    get_next_MID(MID, Group_list, GID);

    /* DEBUG */
    /* printf(">>> next MID = %s|\n", MID); */

    // make message directory
    sprintf(message_dir_path, "GROUPS/%s/MSG/%s", GID, MID);
    mkdir(message_dir_path, 0700);

    if (file_is_being_sent) {

        receive_n_tokens_TCP(2, buffer, fd);        
        sscanf(buffer, "%s %s", FName, Fsize);

        /* DEBUG */
        /* printf(">>> buffer = %s|\n", buffer);
        printf(">>> FName = %s|\n", FName);
        printf(">>> Fsize = %s|\n", Fsize); */

        receive_data_TCP(FName, Fsize, GID, MID, fd);
    }

    /* ^^^ */

    /* DEBUG */
    /* printf("-------\n");
    printf(">>> UID = %s|\n", UID);
    printf(">>> GID = %s|\n", GID);
    printf(">>> MID = %s|\n", MID);
    printf(">>> text = %s|\n", text); */

    make_author_file(UID, GID, MID);
    make_text_file(text, GID, MID);

    increment_last_message_available(Group_list, GID);

    sprintf(reply, "RPT %s\n", MID);
    send_TCP(reply, fd);

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        printf("Request with IP %s on port %s.\n", client_ip, client_port);
    }

    free(reply);
}


void retrieve_command(char *message, int fd, struct sockaddr_in *addr) {

    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    char MID[MAX_SIZE];
    char client_ip[MAX_SIZE];
    char client_port[MAX_SIZE];
    char *reply = (char*)malloc(MAX_REPLY_SIZE);
    char buffer[MAX_SIZE];

    /* process_retrieve_message(message_remainder, reply); */

    receive_n_chars_TCP(14, buffer, fd);
    sscanf(buffer, "%s %s %s", UID, GID, MID);

    /* DEBUG */
    printf("--- MID = %s|\n", MID);

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

    /* DEBUG */
    printf("--- MID = %s|\n", MID);

    retrieve_and_send_messages_TCP(UID, GID, MID, fd);

    /* if (strcmp(reply, "ERR\n") == 0) {
        return;
    } */

    if (verbose_mode) {
        get_client_ip_and_port(fd, client_ip, client_port, addr);
        
        printf("Request with IP %s on port %s.\n", client_ip, client_port);
    }

    free(reply);
}


void process_register_message(char *message, char *reply) {

    int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    
    number_of_tokens = get_number_of_tokens(message);

    /* DEBUG */
    /* printf(">>> number_of_tokens = %d\n", number_of_tokens); */

    // terminate_string_after_n_tokens(message, 3);
    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(message, "%s %s %s", aux, UID, pass);

    if (validate_UID(UID) && validate_pass(pass) && !user_is_registered(UID)) {
        /* DEBUG */
        /* printf(">>> case 1"); */

        if(register_user(UID, pass)) {
            strcpy(reply, "RRG OK\n");
        }
        else {
            strcpy(reply, "RRG NOK\n");
        }
    }
    else if (validate_UID(UID) && validate_pass(pass) && user_is_registered(UID)) {
        /* DEBUG */
        /* printf(">>> case 2"); */

        strcpy(reply, "RRG DUP\n");
    }
    else {
        /* DEBUG */
        /* printf(">>> case 3"); */

        strcpy(reply, "RRG NOK\n");
    }
}


void process_unregister_message(char *message, char *reply) {

    // int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    
    number_of_tokens = get_number_of_tokens(message);

    /* DEBUG */
    /* printf(">>> number_of_tokens = %d\n", number_of_tokens); */

    // terminate_string_after_n_tokens(message, 3);
    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(message, "%s %s %s", aux, UID, pass);

    if (validate_UID(UID) && validate_pass(pass)) {

        if(unregister_user(UID, pass)) {

            strcpy(reply, "RUN OK\n");
        }
        else {
            /* DEBUG */
            /* printf(">>> case 2\n");
            printf(">>> UID = %s|\n", UID);
            printf(">>> pass = %s|\n", pass); */

            strcpy(reply, "RUN NOK\n");
        }
    }
    else {

        strcpy(reply, "RUN NOK\n");
    }
}


void process_login_message(char *message, char *reply) {

    // int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    
    number_of_tokens = get_number_of_tokens(message);

    /* DEBUG */
    /* printf(">>> number_of_tokens = %d\n", number_of_tokens); */

    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(message, "%s %s %s", aux, UID, pass);

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

    // int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char pass[MAX_SIZE];
    
    number_of_tokens = get_number_of_tokens(message);

    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(message, "%s %s %s", aux, UID, pass);

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

    // int n;
    int number_of_tokens;
    int number_of_groups;
    // char aux[MAX_SIZE];
    // char UID[MAX_SIZE];
    // char pass[MAX_SIZE];
    /* GROUPLIST *list; */
    // int number_of_groups;
    char *aux_string;
    
    number_of_tokens = get_number_of_tokens(message);

    if (number_of_tokens != 1) {
        strcpy(reply, "ERR\n");
        return;
    }

    get_groups(Group_list);
    aux_string = GROUPLIST_to_string(Group_list/* , reply */);

    number_of_groups = get_groups(Group_list);
    if (number_of_groups == 0) {
        /* DEBUG */
        printf("--- ECHO\n");

        strcpy(reply, "RGL 0\n");

        /* DEBUG */
        printf("--- reply = %s|\n", reply);
        return;
    }

    sprintf(reply, "RGL %d ", Group_list->no_groups);
    strcat(reply, aux_string);
    free(aux_string);
}


void process_subscribe_message(char *message, char *reply) {

    // int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char GName_aux[MAX_SIZE];
    
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 4) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(message, "%s %s %s %s", aux, UID, GID, GName);
    get_group_name(GID, GName_aux);

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

    if (strcmp(GID, "00") == 0) {

        create_new_group(GID, GName); // GID gets changed to ID of new group
        if (subscribe_user(UID, GID)) {
            sprintf(reply, "RGS NEW %s\n", GID);
        }
        else {
            strcpy(reply, "RGS NOK\n");
        }
        return;
    }
    else {
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

    // int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    
    number_of_tokens = get_number_of_tokens(message);

    // terminate_string_after_n_tokens(message, 3);
    if (number_of_tokens != 3) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(message, "%s %s %s", aux, UID, GID);

    if (!validate_UID(UID)) {
        strcpy(reply, "RGU E_USR\n");
        return;
    }
    if (!validate_GID(GID)) {
        strcpy(reply, "RGU E_GRP\n");
        return;
    }

    if(unsubscribe_user(UID, GID)) {

        strcpy(reply, "RGU OK\n");
    }
    else {

        strcpy(reply, "RGU NOK\n");
    }
}


void process_my_groups_message(char *message, char *reply) {

    // int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char *aux_string;
    // char pass[MAX_SIZE];
    /* GROUPLIST *list; */
    int number_of_groups;
    GROUPLIST *my_groups_list;
    
    terminate_string_after_n_tokens(message, 2);
    number_of_tokens = get_number_of_tokens(message);

    if (number_of_tokens != 2) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(message, "%s %s", aux, UID);

    if (!validate_UID(UID) || !user_is_logged_in(UID)) {
        strcpy(reply, "RGM E_USR\n");
        return;
    }

    my_groups_list = malloc(sizeof(*my_groups_list));
    initialize_group_list(my_groups_list);

    number_of_groups = get_my_groups(my_groups_list, UID);
    if (number_of_groups == 0) {
        /* DEBUG */
        printf("--- ECHO\n");

        strcpy(reply, "RGM 0\n");
        return;
    }

    aux_string = GROUPLIST_to_string(my_groups_list/* , reply */);

    sprintf(reply, "RGM %d ", my_groups_list->no_groups);
    strcat(reply, aux_string);
    free(aux_string);
}


void process_ulist_message(char *message_remainder, char *reply) {

    // int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char *user_list;
    // char *aux_string;
    // char pass[MAX_SIZE];
    /* GROUPLIST *list; */
    // int number_of_groups;
    
    terminate_string_after_n_tokens(message_remainder, 1);
    number_of_tokens = get_number_of_tokens(message_remainder);

    if (number_of_tokens != 1) {
        strcpy(reply, "ERR\n");
        return;
    }

    /* sscanf(message, "%s %s", aux, GID); */
    sscanf(message_remainder, "%s", GID);

    if (!group_exists(GID)) {
        strcpy(reply, "RUL NOK\n");
        return;
    }

    user_list = (char *)malloc(MAX_REPLY_SIZE);
    get_users_of_group(user_list, GID, GName);

    sprintf(reply, "RUL OK %s %s\n", GName, user_list);
}


int validate_post_message(char *UID, char *GID) {

    if (!validate_UID(UID) || !user_is_registered(UID) || !user_is_subscribed_to_group(UID, GID)) {
        return 0;
    }
    if (!validate_GID(GID) || !group_exists(GID)) {
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

    /* DEBUG */
    /* printf(">>> file_paht = %s|\n", file_path); */

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

    /* DEBUG */
    printf(">>> file_path = %s|\n", file_path);

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        return 0;
    }

    return 1;
}


int register_user(char *UID, char *pass) {

    int n;
    char user_dir_path[MAX_SIZE];
    char user_pass_path[MAX_SIZE];
    FILE* fp;

    sprintf(user_dir_path, "USERS/%s", UID);

    // create directory for user UID
    n = mkdir(user_dir_path, 0700);
    if(n == -1) {
        return 0;
    }

    sprintf(user_pass_path, "USERS/%s/%s_pass.txt", UID, UID);
    fp = fopen(user_pass_path, "a");
    validate_fopen(fp);

    n = fprintf(fp, "%s", pass);
    validate_fprintf(n);

    n = fclose(fp);
    validate_fclose(n);
    
    return 1;
}


int unregister_user(char *UID, char *pass) {
    
    int n;
    char user_dir_path[MAX_SIZE];
    char user_pass_path[MAX_SIZE];
    char user_login_path[MAX_SIZE];

    sprintf(user_dir_path, "USERS/%s", UID);
    sprintf(user_pass_path, "USERS/%s/%s_pass.txt", UID, UID);
    sprintf(user_login_path, "USERS/%s/%s_login.txt", UID, UID);

    if (!check_password(pass, user_pass_path)) {
        return 0;
    }

    delete_file(user_pass_path);
    delete_file(user_login_path);

    n = rmdir(user_dir_path);
    if(n == -1) {
        return 0;
    }
    return 1;
}


int login_user(char *UID, char *pass) {

    int n;
    char user_dir_path[MAX_SIZE];
    char user_pass_path[MAX_SIZE];
    char user_login_path[MAX_SIZE];

    sprintf(user_dir_path, "USERS/%s", UID);
    sprintf(user_pass_path, "USERS/%s/%s_pass.txt", UID, UID);
    sprintf(user_login_path, "USERS/%s/%s_login.txt", UID, UID);

    if (!check_password(pass, user_pass_path)) {
        return 0;
    }

    mkdir(user_dir_path, 0700);
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

    if (!check_password(pass, user_pass_path)) {
        return 0;
    }

    delete_file(user_login_path);

    return 1;
}


int subscribe_user(char *UID, char *GID) {

    // int n;
    char group_user_path[MAX_SIZE];

    /* DEBUG */
    printf(">>> subscribe_user: ECHO\n");

    sprintf(group_user_path, "GROUPS/%s/%s.txt", GID, UID);
    make_file(group_user_path);

    return 1;
}


int unsubscribe_user(char *UID, char *GID) {

    // int n;
    char user_group_path[MAX_SIZE];
    // char user_pass_path[MAX_SIZE];
    // char user_login_path[MAX_SIZE];
    FILE *fp;

    sprintf(user_group_path, "GROUPS/%s/%s.txt", GID, UID);

    fp = fopen(user_group_path, "r");
    if (fp == NULL) {
        return 0;
    }

    delete_file(user_group_path);
    return 1;
}


int create_new_group(char *GID, char *GName) {

    int n;
    int group_number;
    char group_dir_path[MAX_SIZE];
    char group_name_path[MAX_SIZE];
    char group_messages_path[MAX_SIZE];
    FILE* fp;

    Number_of_groups = get_groups(Group_list);

    if (Number_of_groups < 0) {
        fprintf(stderr, "ERROR: create_new_group: Invalid number of groups in the system.\n");
        return 0;
    }
    else if (Number_of_groups == 99) {
        fprintf(stderr, "ERROR: create_new_group: Group number limit reached.\n");
        return 0;
    }

    group_number = Number_of_groups + 1;
    if (group_number < 10) {
        sprintf(GID, "0%d", group_number);
    }
    else {
        sprintf(GID, "%d", group_number);
    }

    /* DEBUG */
    printf(">>> newGID = %s\n", GID);

    // create directory for user GID
    sprintf(group_dir_path, "GROUPS/%s", GID);
    n = mkdir(group_dir_path, 0700);
    if(n == -1) {
        /* DEBUG */
        printf(">>> echoo 1\n");

        return 0;
    }

    sprintf(group_name_path, "GROUPS/%s/%s_name.txt", GID, GID);
    fp = fopen(group_name_path ,"a");
    validate_fopen(fp);

    /* DEBUG */
    /* printf(">>> group_name_path = %s|\n", group_name_path);
    printf(">>> GName = %s|\n", GName); */

    n = fprintf(fp, "%s", GName);
    validate_fprintf(n);

    n = fclose(fp);
    validate_fclose(n);

    // create subdirectory for user messages
    sprintf(group_messages_path, "GROUPS/%s/MSG", GID);
    n = mkdir(group_messages_path, 0700);
    if(n == -1) {
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
        /* fclose(fp); */ // ???
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

    long Fsize;
    FILE *fp;
    char *read_pass;

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


int get_groups(GROUPLIST *list) {

    DIR *d;
    struct dirent *dir;
    int i = 0;
    FILE *fp;
    char GIDname[MAX_SIZE];
    
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


int get_my_groups(GROUPLIST *list, char *UID) {

    DIR *d;
    struct dirent *dir;
    int i=0;
    FILE *fp;
    char GIDname[MAX_SIZE];
    char group_user_path[MAX_SIZE];
    
    list->no_groups=0;
    d = opendir("GROUPS");

    /* DEBUG */
    printf("ECHO1\n");

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.') {
                continue;
            }
            if (strlen(dir->d_name)>2) {
                continue;
            }

            sprintf(group_user_path, "GROUPS/%s/%s.txt", dir->d_name, UID);

            /* DEBUG */
            printf(">>> group_user_path = %s|\n", group_user_path);
            
            fp = fopen(group_user_path, "r");
            if (fp == NULL) {
                continue;
            }

            /* DEBUG */
            /* printf(">>> group no = %s|\n", list->group_no[i]); */

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
        /* DEBUG */
        printf("ECHO2\n");

        return(-1);
    }

    if(list->no_groups>1) {
        SortGList(list);
    }

    /* DEBUG */
    printf("--- no_groups = %d\n", list->no_groups);

    return(list->no_groups);
}


int get_users_of_group(char *user_list, char *GID, char *GName) {

    int number_of_subscribed_users = 0;
    char group_path[40];
    char UID[20];
    DIR *d;
    struct dirent *dir;

    if (!group_exists(GID)) {
        return 0;
    }
    get_group_name(GID, GName);
    /* DEBUG */
    printf(">>> GName = %s\n", GName);

    /* terminate_string_after_n_tokens(GName, 1); */

    /* DEBUG */
    printf(">>> GName = %s\n", GName);

    strcpy(user_list, "");
    sprintf(group_path, "GROUPS/%s", GID);

    d = opendir(group_path);
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[5] == '.') {

            get_UID_from_file_name(UID, dir->d_name);

            /* sscanf(dir->d_name, "%s.txt", UID); */
            strcat(user_list, UID);
            strcat(user_list, " ");
            number_of_subscribed_users++;
        }
    }
    return number_of_subscribed_users;
}


int get_group_name(char *GID, char *GName) {

    FILE *fp;
    char group_name_path[MAX_SIZE];
    int Fsize;

    if (!group_exists(GID)) {
        return 0;
    }

    sprintf(group_name_path, "GROUPS/%s/%s_name.txt", GID, GID);

    fp = fopen(group_name_path, "r");
    Fsize = get_file_size(fp);
    /* DEBUG */
    printf(">>> Fsize = %d\n", Fsize);

    fread(GName, Fsize, 1, fp);
    GName[2] = '\0';

    return 1;
}


void initialize_group_list(GROUPLIST *list) {

    for (int i = 0; i < MAX_GROUPS; i++) {
        list->number_of_messages[i] = 0;
        strcpy(list->last_message_available[i], "0000");
    }
}


void SortGList(GROUPLIST *list) {

    int i, j;
    int n;

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
    
    /* sprintf(reply, "RGL %d ", list->no_groups);
    strcat(reply, aux_string); */
    /* free(aux_string); */
}


void increment_last_message_available(GROUPLIST *list, char *GID) {

    int i;
    int last_message_available_int;
    int new_last_message_available_int;
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


void get_next_MID(char *MID, GROUPLIST *list, char *GID) {

    int i;
    int last_message_available_int;
    int new_last_message_available_int;
    char last_message_available[10];
    char new_last_message_available[10];

    i = get_index(list, GID);

    /* DEBUG */
    /* printf("@@@ index = %d\n", i); */

    last_message_available_int = atoi(list->last_message_available[i]);
    new_last_message_available_int = last_message_available_int + 1;

    /* DEBUG */
    /* printf("@@@ last_message_available_int = %d\n", last_message_available_int);
    printf("@@@ next_message_available_int = %d\n", new_last_message_available_int); */

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
        fprintf(stderr, "@@@ ERROR: increment_last_message_available\n");
    }

    /* DEBUG */
    /* printf("@@@ new_last_message_available = %s\n", new_last_message_available); */

    strcpy(MID, new_last_message_available);
}

void increment_MID(char *MID) {

    int MID_int;
    int new_MID_int;
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


void update_last_available_message(GROUPLIST *list, char *GID, int i) {

    int largest_MID_int = 0;
    int current_MID_int;
    DIR *d;
    struct dirent *dir;
    char group_messages_path[40];
    char largest_MID[10];
    // int i = 0;
    // FILE *fp;
    // char GIDname[30];
    
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

        convert_GID_int_to_string(largest_MID_int, largest_MID);
        strcpy(list->last_message_available[i], largest_MID);
    }
}


void convert_GID_int_to_string(int MID_int, char *MID) {

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
        fprintf(stderr, "ERROR: convert_GID_int_to_string\n");
    }
}


int  get_index(GROUPLIST *list, char *GID) {

    int i;
    int n = list->no_groups;

    for (i = 0; i < n; i++) {
        if (strcmp(GID, list->group_no[i]) == 0) {
            return i;
        }
    }
    return -1;
}


void get_UID_from_file_name(char *UID, char *file_name) {

    UID[0] = file_name[0];
    UID[1] = file_name[1];
    UID[2] = file_name[2];
    UID[3] = file_name[3];
    UID[4] = file_name[4];
}


void get_client_ip_and_port(int fd, char *client_ip, char *client_port, struct sockaddr_in *addr) {

    struct in_addr ip_addr = addr->sin_addr;
    uint16_t port = 0;

    inet_ntop(AF_INET, &ip_addr, client_ip, INET_ADDRSTRLEN);
    port = htons(addr->sin_port);
    sprintf(client_port, "%d", port);
}


int create_socket_datagram() {

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("ERROR: socket: create_socket_datagram\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}


void get_address_info_datagram(struct addrinfo *hints, struct addrinfo **res, char *port) {

    memset(&(*hints), 0, sizeof((*hints))); 
    (*hints).ai_family = AF_INET;        
    (*hints).ai_socktype = SOCK_DGRAM;   
    (*hints).ai_flags = AI_PASSIVE;

    int errcode = getaddrinfo(NULL, port, &(*hints), &(*res));
    if(errcode != 0) {
        perror("ERROR: getaddrinfo: get_address_info_datagram\n");
        exit(EXIT_FAILURE);
    }
}


int create_socket_stream() {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("ERROR: socket: create_socket_stream\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}


void get_address_info_stream(struct addrinfo *hints, struct addrinfo **res, char *port) {

    memset(&(*hints), 0, sizeof((*hints))); 
    (*hints).ai_family = AF_INET;        
    (*hints).ai_socktype = SOCK_STREAM;  
    (*hints).ai_flags = AI_PASSIVE;

    int errcode = getaddrinfo(NULL, port, &(*hints), &(*res));
    if(errcode != 0) {
        perror("ERROR: getaddrinfo: get_address_info_stream\n");
        exit(EXIT_FAILURE);
    }
}


/* void create_UDP_server_socket() {

    fd_UDP = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
    if(fd_UDP == -1) {
        perror("ERROR: socket: create_UDP_socket\n");
        exit(EXIT_FAILURE);
    }
} */


/* void get_address_info_server_UDP() {

    memset(&hints_UDP, 0, sizeof(hints_UDP)); 
    hints_UDP.ai_family = AF_INET;        // IPv4
    hints_UDP.ai_socktype = SOCK_DGRAM;   // UDP socket

    errcode = getaddrinfo(NULL, PORT, &hints_UDP, &res_UDP);
    if(errcode != 0) {
        perror("ERROR: get_address_info_UDP\n");
        exit(EXIT_FAILURE);
    }
} */


/* void create_TCP_server_socket() {

    fd_TCP = socket(AF_INET, SOCK_STREAM, 0); // TCP Socket
    if(fd_TCP == -1) {
        perror("ERROR: create_TCP_socket: can't open socket.\n");
        exit(EXIT_FAILURE);
    }
} */


/* void get_address_info_server_TCP() {

    memset(&hints_TCP, 0, sizeof(hints_TCP)); 
    hints_TCP.ai_family = AF_INET;        // IPv4
    hints_TCP.ai_socktype = SOCK_STREAM;  // TCP socket

    errcode = getaddrinfo(NULL, PORT, &hints_TCP, &res_TCP);
    if(errcode != 0) {
        perror("ERROR: get_address_info_TCP\n");
        exit(EXIT_FAILURE);
    }
} */


void receive_message_UDP(int fd, char *message, struct sockaddr_in *addr) {

    int ret;
    /* struct sockaddr_in addr; */
    socklen_t addrlen ;
    
    addrlen = sizeof(*addr);
    ret = recvfrom(fd, message, MAX_SIZE, 0, (struct sockaddr*)&(*addr), &addrlen);
    validate_recvfrom(ret);

    message[ret] = '\0';
}


/* void receive_message_TCP(int fd, char *message) {

    int ret;
    char *ptr;

    message[0] = '\0';
    
    ptr = message;
    while (1) {
        
        ret = read(fd, ptr, 1);
        validate_read(ret);

        if (*ptr == '\n') {
            break;
        }

        ptr++;
    }
    *ptr = '\0';
} */


/* void receive_keyword_TCP(char *keyword, int fd) {

    int n;
    int bytes_to_read = 10 * sizeof(char);
    int read_bytes = 0;

    keyword[0] = '\0';

    n = read(fd, keyword, bytes_to_read);
    validate_read(n);
} */


void receive_n_chars_TCP(int n, char *string, int fd) {

    int ret;
    int bytes_to_read = n * sizeof(char);
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
 * Returns 1 if char after nth char is ' ', 0 if it is '\n' and -1 otherwise 
 **/
int receive_n_plus_1_chars_TCP(int n, char *string, int fd) {

    int ret = 0;
    int bytes_to_read = (n + 1) * sizeof(char);
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


void send_n_chars_TCP(int n, char *string, int fd) {
    
    int ret;
    int bytes_to_write = n * sizeof(char);

    while (bytes_to_write > 0) {

        ret = write(fd, string, bytes_to_write);
        validate_read(ret);

        bytes_to_write -= ret;
        string += ret;
    }
}


void receive_n_tokens_TCP(int n, char *string, int fd) {

    int ret;
    int spaces_to_read = n;
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


void receive_data_TCP(char *FName, char *Fsize, char *GID, char *MID, int fd) {

    int   ret;
    int   bytes_to_read = atoi(Fsize);
    char  file_path[MAX_SIZE];
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


void send_reply_UDP(char *reply, int fd, struct sockaddr_in *addr) {

    int n;
    socklen_t addrlen;
    
    addrlen = sizeof((*addr));
    n = sendto(fd, reply, strlen(reply), 0, (struct sockaddr*)&(*addr), addrlen);
    validate_sendto(n);
}


/* void send_reply_TCP(char *reply, int fd) {

    int n;
    int bytes_to_write = strlen(reply) * sizeof();

    while (bytes_to_write > 0) {
        
        n = write(fd, reply, strlen(reply));
        validate_write(n);

        bytes_to_write -= n;
        reply += n;
    }
} */


void send_TCP(char *string, int fd) {

    int ret;
    int bytes_to_write = strlen(string) * sizeof(char);
    char *ptr;

    while (bytes_to_write > 0) {
        
        ret = write(fd, string, bytes_to_write);
        validate_write(ret);

        bytes_to_write -= ret;
        string += ret;
    }
}


void retrieve_and_send_messages_TCP(char *UID, char *GID, char *MID, int fd) {

    int  N;
    int  Tsize_int;
    int  Fsize_int;
    int  number_of_files = 0;
    int read_messages = 0;
    char group_message_path[40];
    char group_messages_path[40];
    char *reply_aux = (char *)malloc(MAX_REPLY_SIZE);
    char buffer[MAX_SIZE];
    char reply[MAX_SIZE];
    char FName[MAX_SIZE];
    char Fsize[MAX_SIZE];
    char Tsize[MAX_SIZE];
    char text[MAX_TEXT_SIZE];
    char file_path[MAX_SIZE];
    DIR  *d;
    FILE *fp;
    struct dirent *dir;

    if (!group_exists(GID)) {
        strcpy(reply, "RRT NOK\n");
        send_TCP(reply, fd);
        return;
    }

    sprintf(group_messages_path, "GROUPS/%s/MSG", GID);
    N = get_number_of_files(group_messages_path);

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

        read_text_from_file(text, file_path, Tsize_int);

        if (number_of_files == 3) { // if there is file associated with message

            d = opendir(group_message_path);
            while ((dir = readdir(d)) != NULL) {

                if (dir->d_name[0] == '.') {
                    continue;
                }
                if (strcmp(dir->d_name, "A U T H O R.txt") == 0) {
                    
                    sprintf(file_path, "GROUPS/%s/MSG/%s/A U T H O R.txt", GID, MID);
                    read_text_from_file(UID, file_path, 5);

                    /* DEBUG */
                    printf("!!! UID = %s|\n", UID);
                }
                if (strcmp(dir->d_name, "A U T H O R.txt") && strcmp(dir->d_name, "T E X T.txt")) {
                    strcpy(FName, dir->d_name);
                }
            }

            sprintf(file_path, "GROUPS/%s/MSG/%s/%s", GID, MID, FName);
            Fsize_int = get_file_size_char(file_path);
            sprintf(Fsize, "%d", Fsize_int);

            sprintf(reply_aux, "%s %s %s %s / %s %s ", MID, UID, Tsize, text, FName, Fsize);
            
            send_TCP(reply_aux, fd);
            
            sprintf(file_path, "GROUPS/%s/MSG/%s/%s", GID, MID, FName);
            send_data_TCP(file_path, Fsize, fd);

            /* DEBUG */
            /* printf(">>> file_path2 = %s|\n", file_path); */

            /* strcpy(aux, " ");
            send_n_chars_TCP(1, aux, fd); */

            /* DEBUG */
            printf("=== FILE\n");
            printf("=== FName = %s|\n", FName);
            printf("=== Fsize = %s|\n", Fsize);
        }
        else {

            d = opendir(group_message_path);
            while ((dir = readdir(d)) != NULL) {

                if (dir->d_name[0] == '.') {
                    continue;
                }
                if (strcmp(dir->d_name, "A U T H O R.txt") == 0) {
                    
                    sprintf(file_path, "GROUPS/%s/MSG/%s/A U T H O R.txt", GID, MID);
                    read_text_from_file(UID, file_path, 5);

                    /* DEBUG */
                    printf("!!! UID = %s|\n", UID);
                }
            }

            sprintf(reply_aux, "%s %s %s %s", MID, UID, Tsize, text);
            send_TCP(reply_aux, fd);
        }

        read_messages++;
        if (read_messages == 20) {
            break;
        }

        /* DEBUG */
        printf("=== MID = %s|\n", MID);
        printf("=== UID = %s|\n", UID);
        printf("=== Tsize = %s|\n", Tsize);
        printf("=== text = %s|\n", text);
        printf("-----------------\n");

        increment_MID(MID); 
        sprintf(group_message_path, "GROUPS/%s/MSG/%s", GID, MID);
        
        fp = fopen(group_message_path, "r");
    }

    strcpy(buffer, "\n");
    send_TCP(buffer, fd);
    free(reply_aux);
}


void send_data_TCP(char *file_path, char *Fsize, int fd) {

    int ret;
    int bytes_to_write;
    char *buffer;
    FILE *fp;

    fp = fopen(file_path, "r");
    validate_fopen(fp);

    while (!feof(fp)) {

        buffer = (char *)malloc(512);
        bytes_to_write = fread(buffer, 1, 512, fp);

        while (bytes_to_write > 0) {

            ret = write(fd, buffer, bytes_to_write);
            validate_write(ret);

            bytes_to_write -= ret;
            buffer += ret;
        }
    }

    fclose(fp);
    /* free(buffer); */
}


void make_file(char *file_path) {

    FILE *fp;

    fp = fopen(file_path ,"a");
    /* validate_fopen(fp); */
    /* fclose(fp); */ // ???
}


void make_author_file(char *UID, char *GID, char *MID) {

    char author_file_path[MAX_SIZE];
    FILE *fp;
    int size = 5 * sizeof(char);

    sprintf(author_file_path, "GROUPS/%s/MSG/%s/A U T H O R.txt", GID, MID);

    fp = fopen(author_file_path, "w");
    validate_fopen(fp);

    /* fprintf(fp, "%s", UID); */
    fwrite(UID, size, 1, fp);

    fclose(fp);
}


void make_text_file(char *text, char *GID, char *MID) {

    char text_file_path[MAX_SIZE];
    FILE *fp;
    int Tsize = strlen(text) * sizeof(char);

    sprintf(text_file_path, "GROUPS/%s/MSG/%s/T E X T.txt", GID, MID);

    fp = fopen(text_file_path, "w");
    validate_fopen(fp);
    /* fprintf(fp, "%s", text); */
    fwrite(text, Tsize, 1, fp);

    fclose(fp);
}




void delete_file(char *file_path) {

    unlink(file_path);
}


int get_number_of_files(char *dir_path) {

    int number_of_files = 0;
    DIR *d;
    struct dirent *dir;

    d = opendir(dir_path);
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] == '.') {
            continue;
        }
        number_of_files++;
    }
    return number_of_files;
}


void read_text_from_file(char *text, char *file_path, int size) {

    FILE *fp;

    fp = fopen(file_path, "r");
    fread(text, size, 1, fp);

    text[size] = '\0';

    fclose(fp);
}
