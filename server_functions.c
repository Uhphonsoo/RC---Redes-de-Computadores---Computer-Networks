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
#include "server_functions.h"

extern int  verbose_mode;
extern int Number_of_groups;
extern GROUPLIST *Group_list;
/* extern socklen_t addrlen_UDP;
extern struct sockaddr_in addr_UDP; */
// extern char DSIP[MAX_SIZE];
// extern char DSport[MAX_SIZE];
// extern char Message[MAX_SIZE];
// extern char reply[MAX_REPLY_SIZE];

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
    
    process_groups_message(message, reply);

    // communication with server
    send_reply_UDP(reply, fd, addr);

    /* DEBUG */
    printf(">>> reply = %s\n", reply);

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

    /* DEBUG */
    printf(">>> reply = %s\n", reply);

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

    // TODO
}


void ulist_command(char *message, int fd, struct sockaddr_in *addr) {

    // TODO
}


void post_command(char *message, int fd, struct sockaddr_in *addr) {

    // TODO
}


void retrieve_command(char *message, int fd, struct sockaddr_in *addr) {

    // TODO
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
            /* DEBUG */
            printf(">>> case 1\n");

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
        /* DEBUG */
        printf(">>> case 3\n");

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
    // char aux[MAX_SIZE];
    // char UID[MAX_SIZE];
    // char pass[MAX_SIZE];
    /* GROUPLIST *list; */
    int number_of_groups;
    
    number_of_tokens = get_number_of_tokens(message);

    if (number_of_tokens != 1) {
        strcpy(reply, "ERR\n");
        return;
    }

    number_of_groups = get_groups(Group_list);
    GROUPLIST_to_string(Group_list, reply);
}


void process_subscribe_message(char *message, char *reply) {

    // int n;
    int number_of_tokens;
    char aux[MAX_SIZE];
    char UID[MAX_SIZE];
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    
    number_of_tokens = get_number_of_tokens(message);
    if (number_of_tokens != 4) {
        strcpy(reply, "ERR\n");
        return;
    }

    sscanf(message, "%s %s %s %s", aux, UID, GID, GName);

    if (!validate_UID(UID)) {
        strcpy(reply, "RGS E_USR\n");
        return;
    }
    else if (!validate_GID(GID)) {
        strcpy(reply, "RGS E_GRP\n");
        return;
    }
    else if (!validate_GName(GName)) {
        strcpy(reply, "RGS E_GNAME\n");
        return;
    }
    else if (Number_of_groups == 99) {
        strcpy(reply, "RGS E_FULL\n");
        return;
    }

    /* DEBUG */
    printf(">>> echo 1\n");

    if (strcmp(GID, "00") == 0) {

        /* DEBUG */
        printf(">>> echo 2\n");

        create_new_group(GID, GName); // GID gets changed to ID of new group
        if (subscribe_user(UID, GID)) {
            sprintf(reply, "RGS NEW %s", GID);
            /* strcpy(reply, "RGS NEW %s\n", GID); */
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

    /* DEBUG */
    /* printf(">>> number_of_tokens = %d\n", number_of_tokens); */

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
        /* DEBUG */
        /* printf(">>> case 1\n"); */

        strcpy(reply, "RGU OK\n");
    }
    else {
        /* DEBUG */
        /* printf(">>> case 2\n");
        printf(">>> UID = %s|\n", UID);
        printf(">>> pass = %s|\n", pass); */

        strcpy(reply, "RGU NOK\n");
    }
}


int user_is_registered(char *UID) {

    char file_path[MAX_SIZE];
    FILE *fp;

    sprintf(file_path, "USERS/%s", UID);
    fp = fopen(file_path, "r");

    /* DEBUG */
    printf(">>> file_paht = %s|\n", file_path);

    if (fp == NULL) {
        return 0;
    }
    else {
        return 1;
    }
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

    /* DEBUG */
    printf(">>> user_pass_path = %s|\n", user_pass_path);
    printf(">>> pass = %s|\n", pass);

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
        /* DEBUG */
        printf(">>> echoo 2\n");

        return 0;
    }

    Number_of_groups++;
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
    int i=0;
    FILE *fp;
    char GIDname[30];
    
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


void SortGList(GROUPLIST *list) {

    // TODO
}


void GROUPLIST_to_string(GROUPLIST *list, char *reply) {

    int i = 0;
    char *aux_string = (char *)malloc(MAX_REPLY_SIZE);



    /* DEBUG */
    printf(">>> ECHO: GROUPLIST_to_string\n");

    strcpy(aux_string, "");

    for (i = 0; i < list->no_groups; i++) {
        strcat(aux_string, list->group_no[i]);
        strcat(aux_string, " ");
        strcat(aux_string, list->group_name[i]);
        strcat(aux_string, " ");
        strcat(aux_string, list->last_message_available[i]);
        strcat(aux_string, " ");
    }
    
    sprintf(reply, "RGL %d ", list->no_groups);

    strcat(reply, aux_string);
}


void get_client_ip_and_port(int fd, char *client_ip, char *client_port, struct sockaddr_in *addr) {

    int n;
    // struct sockaddr_in addr;
    socklen_t addrlen;
    addrlen = sizeof((*addr));

    /* n = getpeername(fd, (struct sockaddr *)&addr_, &addr_size_);
    if (n == -1) {
        perror("ERROR: getpeername: get_client_ip\n");
        exit(EXIT_FAILURE);
    } */

    getsockname(fd, (struct sockaddr *) &(*addr), &addrlen);
    // TODO: validate_getsockname();

    strcpy(client_ip, inet_ntoa((*addr).sin_addr));
    sprintf(client_port, "%d", (*addr).sin_port);

    /* strcpy(client_ip, inet_ntoa((*addr).sin_addr));
    sprintf(client_port, "%d", (*addr).sin_port); */
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

    int n;
    /* struct sockaddr_in addr; */
    socklen_t addrlen ;
    
    addrlen = sizeof((*addr));
    n = recvfrom(fd, message, MAX_SIZE, 0, (struct sockaddr*)&(*addr), &addrlen);
    
    validate_recvfrom(n);
}


void receive_message_TCP(int fd, char *message) {

    int n;
    socklen_t addrlen ;
    struct sockaddr_in addr;

    addrlen = sizeof(addr);
    n = read(fd, message, MAX_SIZE);
    validate_read(n);
}


void send_reply_UDP(char *reply, int fd, struct sockaddr_in *addr) {

    int n;
    socklen_t addrlen;
    
    addrlen = sizeof((*addr));
    n = sendto(fd, reply, strlen(reply), 0, (struct sockaddr*)&(*addr), addrlen);
    validate_sendto(n);
}


void make_file(char *file_path) {

    FILE *fp;

    fp = fopen(file_path ,"a");
    /* validate_fopen(fp); */
}

void delete_file(char *file_path) {

    unlink(file_path);
}
