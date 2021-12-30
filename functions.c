#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <ctype.h>
#include "functions.h"
#include "constants.h"

extern int  fd_UDP, fd_TCP;
extern int  errcode;
extern int  logged_in;
extern int  has_active_group;
extern char message[MAX_SIZE];
extern char reply[MAX_REPLY_SIZE];
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

void validate_sendto(int n) {

    if(n == -1) {
        perror("ERROR: sendto: Unable to send message.\n");
        exit(EXIT_FAILURE);
    }
    return;
}


void validate_recvfrom(int n) {

    if(n == -1) {
        perror("ERROR: recvfrom: Unable to receive message from server.\n");
        exit(EXIT_FAILURE);
    }
    return;
}

void validate_connect(int n) {

    if(n == -1) {
        perror("ERROR: connect: Unable to connect to server.\n");
        exit(EXIT_FAILURE);
    }
    return;
}


void validate_write(int n) {

    if(n == -1) {
        perror("ERROR: write: Unable to write message to server.\n");
        exit(EXIT_FAILURE);
    }
    return;
}


void validate_read(int n) {

    if(n == -1) {
        perror("ERROR: read\n");
        exit(EXIT_FAILURE);
    }
    return;
}


void  validate_accept(int fd) {

    if (fd == -1) {
        perror("ERROR: accept\n");
        exit(EXIT_FAILURE);
    }
}



int validate_UID(char* UID) {

    int length = strlen(UID);

    if (length != 5) {
        /* fprintf(stderr, "Invalid user ID.\n"); */
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
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (!(isalpha(pass[i]) || isdigit(pass[i]))) {
            return 0;
        }
    }
    return 1;
}

int validate_GID(char* GID) {

    int length = strlen(GID);

    if (strlen(GID) != 2) {
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
        /* fprintf(stderr, "> Invalid group name."); */
        return 0;
    }
    for (int i = 0; i < length; i++) {
        if (!(isalpha(GName[i]) || isdigit(GName[i]) || GName[i] != '-' || GName[i] != '_')) {
            return 0;
        }
    }
    return 1;
}


int  validate_MID(char* MID) {

    int length = strlen(MID);

    if (length != 4) {
        /* fprintf(stderr, "> Invalid message ID.\n"); */
        return 0;
    }

    if (!(isdigit(MID[0]) && isdigit(MID[1]) && isdigit(MID[2]) && isdigit(MID[3]))) {
        /* fprintf(stderr, "> Invalid message ID.\n"); */
        return 0;
    }
    return 1;
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

int get_nth_token(char* string, int n, char* ret) {

    int i = 0;
    int j = 1;
    int number_of_tokens = get_number_of_tokens(string);

    if (n > number_of_tokens) {
        fprintf(stderr, "get_nth_token: string doesn't have that many tokens\n");
        return - 1;
    }

    if (n == 0) {
        fprintf(stderr, "ERROR: get_nth_token(): invalid input.\n");
        return -1;
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
    // return the position of the last element of the nth token
    return i - 1;
}

int get_number_of_tokens(char* string) {

    int ret = 0;
    int length = strlen(string);
    int i = 0;
    int last_read_character_was_space = 0;

    for (i = 0; i < length; i++) {
        if (i != 0 && !isspace(string[i]) && last_read_character_was_space) {
            if (isalpha(string[i]) || isdigit(string[i])) {
                ret++;
            }
        }
        if (i == 0 && !isspace(string[i])) {
            if (isalpha(string[i]) || isdigit(string[i])) {
                ret++;
            }
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


int get_next_token(char* string, int i, char* ret) {

    int j = 0;

    if (i > strlen(string) - 1 || i < 0 || string == NULL) {
        fprintf(stderr, "ERROR: get_next_token: invalid input.\n");
        return -1;
    }

    while(isspace(string[i])) {
        i++;
    }

    while(!isspace(string[i])) {
        ret[j++] = string[i++];
    }
    ret[j] = '\0';
    return i;
}


int  get_file_size(FILE *fp) {

    long Fsize;

    if (fp) {
        if (fseek(fp, 0, SEEK_END)) {
            perror("ERROR: fseek\n");
            exit(EXIT_FAILURE);
        }

        Fsize = ftell(fp);
        if (Fsize == -1) {
            perror("ERROR: ftell\n");
            exit(EXIT_FAILURE);
        }

        if(fseek(fp, 0, SEEK_SET)) {
            perror("ERROR: fseek\n");
            exit(EXIT_FAILURE);
        }
    }
    return Fsize;
}


void show_groups(char* reply, char* N_string) {

    int N = atoi(N_string);
    int i = 0;
    char GID[MAX_SIZE];
    char GName[MAX_SIZE];
    char MID[MAX_SIZE];

    for (i = 3; i < 3*N + 1; i++) {
        get_nth_token(reply, i++, GID);
        get_nth_token(reply, i++, GName);
        get_nth_token(reply, i, MID);

        printf("> Group ID: %s | Group Name: %s | Message ID: %s\n", GID, GName, MID);
    }
    return;
}


void show_users(char* reply) {

    int i = 3;
    int number_of_tokens = get_number_of_tokens(reply);
    char GName[MAX_SIZE];
    char UID[MAX_SIZE];

    if (number_of_tokens < 3) {
        fprintf(stderr, "ERROR: show_users: invalid input\n");
        return;
    }

    get_nth_token(reply, i++, GName);
    printf("> Group name: %s\n", GName);
    printf(">> Subscribed users:\n");

    while (i <= number_of_tokens) {
        get_nth_token(reply, i++, UID);
        printf(">> User ID: %s\n", UID);
    }
}


// !!! "data" isn't being used
void  show_messages(char* reply) {

    int N = 0;
    int i = 4, j = 0, k = 0;
    int length = strlen(reply);
    int Tsize = 0;
    int Fsize = 0;
    char N_string[MAX_SIZE];
    char MID[MAX_SIZE];
    char UID[MAX_SIZE];
    char Tsize_string[MAX_SIZE];
    char text[MAX_TEXT_SIZE];
    char Fname[MAX_SIZE];
    char Fsize_string[MAX_SIZE];
    char *data;

    i = get_nth_token(reply, 3, N_string);
    N = atoi(N_string);

    if (N == 0) {
        return;
    }

    i++;
    for (k = 0; k < N; k++) {
        i = get_next_token(reply, i, MID); /* get_nth_token(reply, i++, MID); */
        i = get_next_token(reply, i, UID); /* get_nth_token(reply, i++, UID); */
        i = get_next_token(reply, i, Tsize_string); /* get_nth_token(reply, i, Tsize_string); */
        Tsize = atoi(Tsize_string);

        // position i on top of text
        i++;
        // get text
        for (j = 0; j < Tsize; j++) {
            text[j] = reply[i++];
        }

        // if message has file
        i++;
        if (reply[i] == '/' && i < length - 1) {

            i += 2;
            i = get_next_token(reply, i, Fname);
            i = get_next_token(reply, i, Fsize_string);
            Fsize = atoi(Fsize_string);

            i++;
            data = (char*)malloc(Fsize);
            for (j = 0; j < Fsize; j++) {
                data[j] = reply[i++];
            }

            // strip \n from ending of text
            if (text[Tsize-1] == '\n') {
                text[Tsize-1] = '\0';
            }

            printf("> Message ID: %s | From user: %s | Message: %s\n", MID, UID, text);
            printf(">> Associated file: %s | File size: %s\n", Fname, Fsize_string);

            // !!! "data isn't being used"
            free(data);
        }
        else {

            // strip \n from ending of text
            if (text[Tsize-1] == '\n') {
                text[Tsize-1] = '\0';
            }
            printf("> Message ID: %s | From user: %s | Message: %s\n", MID, UID, text);
        }
    }
}


void clear_string(char* string) {

    if (string != NULL) {
        string[0] = '\0';
    }
}


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

void clear_message_and_reply() {
    clear_string(message);
    clear_string(reply);
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