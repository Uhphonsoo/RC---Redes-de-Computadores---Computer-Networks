#ifndef _SERVER_FUNCTIONS_
#define _SERVER_FUNCTIONS_

#include "constants.h"

typedef struct GROUPLIST_tag {
    int no_groups;
    int number_of_messages[MAX_GROUPS];
    char group_no[MAX_GROUPS][20];
    char group_name[MAX_GROUPS][30];
    char last_message_available[MAX_GROUPS][5];
} GROUPLIST;

void validate_program_input(int argc, char **argv, char *DSport);
void process_message(char *message, int fd, struct sockaddr_in *addr);
void process_keyword(char *keyword, int fd, struct sockaddr_in *addr);

void register_command(char *message, int fd, struct sockaddr_in *addr);
void unregister_command(char *message, int fd, struct sockaddr_in *addr);
void login_command(char *message, int fd, struct sockaddr_in *addr);
void logout_command(char *message, int fd, struct sockaddr_in *addr);
void groups_command(char *message, int fd, struct sockaddr_in *addr);
void subscribe_command(char *message, int fd, struct sockaddr_in *addr);
void unsubscribe_command(char *message, int fd, struct sockaddr_in *addr);
void my_groups_command(char *message, int fd, struct sockaddr_in *addr);
void ulist_command(char *message, int fd, struct sockaddr_in *addr);
void post_command(char *message, int fd, struct sockaddr_in *addr);
void retrieve_command(char *message, int fd, struct sockaddr_in *addr);

void process_register_message(char *message, char *reply);
void process_unregister_message(char *message, char *reply);
void process_login_message(char *message, char *reply);
void process_logout_message(char *message, char *reply);
void process_groups_message(char *message, char *reply);
void process_subscribe_message(char *message, char *reply);
void process_unsubscribe_message(char *message, char *reply);
void process_my_groups_message(char *message, char *reply);
void process_ulist_message(char *message, char *reply);

int validate_post_message(char *UID, char *GID);
int validate_retrieve_message(char *UID, char *GID, char *MID);

int user_is_registered(char *UID);
int user_is_logged_in(char *UID);
int register_user(char *UID, char *pass);
int unregister_user(char *UID, char *pass);
int login_user(char *UID, char *pass);
int logout_user(char *UID, char *pass);
int subscribe_user(char *UID, char *GID);
int unsubscribe_user(char *UID, char *GID);
int create_new_group(char *GID, char *GName);
int group_exists(char *GID);
int group_has_messages(char *GID, char *MID);

int check_password(char *pass, char *user_pass_path);
int get_groups(GROUPLIST *list);
int get_my_groups(GROUPLIST *list, char *UID);
int get_users_of_group(char *user_list, char *GID, char *GName);
int get_group_name(char *GID, char *GName);

// GROUPLIST related functions
void  initialize_group_list(GROUPLIST *list);
void  SortGList(GROUPLIST *list);
void  swap_groups(int g1, int g2, GROUPLIST *list);
char *GROUPLIST_to_string(GROUPLIST *list/* , char *reply */);
void  increment_last_message_available(GROUPLIST *list, char *GID);
int   get_index(GROUPLIST *list, char *GID);
void  get_next_MID(char *MID, GROUPLIST *list, char *GID);
void  increment_MID(char *MID);
int   get_number_of_messages(char *GID);
void  update_last_available_message(GROUPLIST *list, char *GID, int i);
void convert_GID_int_to_string(int MID_int, char *MID);

// socket related functions
int  create_socket_datagram();
void get_address_info_datagram(struct addrinfo *hints, struct addrinfo **res, char *port);
int  create_socket_stream();
void get_address_info_stream(struct addrinfo *hints, struct addrinfo **res, char *port);
void receive_message_UDP(int fd, char *message, struct sockaddr_in *addr);
// void receive_message_TCP(int fd, char *message); // !!!
// void receive_keyword_TCP(char *keyword, int fd); // !!!
void receive_n_chars_TCP(int n, char *string, int fd); // ccc
int  receive_n_plus_1_chars_TCP(int n, char *string, int fd); // ccc
void receive_n_tokens_TCP(int n, char *string, int fd); // ccc
void receive_data_TCP(char *FName, char *Fsize, char *GID, char *MID, int fd); // ccc
void send_n_chars_TCP(int n, char *string, int fd); // ccc
void send_reply_UDP(char *reply, int fd, struct sockaddr_in *addr);
// void send_reply_TCP(char *reply, int fd); // !!!
void send_TCP(char *string, int fd); // ccc
void retrieve_and_send_messages_TCP(char *UID, char *GID, char *MID, int fd); // ccc
void send_data_TCP(char *FName, char *Fsize, int fd); // ccc

// file related functions
void make_file(char *file_path);
void make_author_file(char *UID, char *GID,char *MID);
void make_text_file(char *text, char *GID, char *MID);
void delete_file(char *file_path);
int  get_number_of_files(char *dir_path);
void read_text_from_file(char *text, char *file_path, int size);
void get_UID_from_file_name(char *UID, char *file_name);


void get_client_ip_and_port(int fd, char *client_ip, char *client_port, struct sockaddr_in *addr);

#endif