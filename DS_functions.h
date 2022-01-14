#ifndef _SERVER_FUNCTIONS_
#define _SERVER_FUNCTIONS_

#include "constants.h"

typedef struct GROUPLIST_tag {
    int  no_groups;
    int  number_of_messages[MAX_GROUPS];
    char group_no[MAX_GROUPS][20];
    char group_name[MAX_GROUPS][30];
    char last_message_available[MAX_GROUPS][5];
} GROUPLIST;

// core input functions
void validate_program_input(int argc, char **argv, char *DSport);
void process_message(char *message, int fd, struct sockaddr_in *addr);
void process_keyword(char *keyword, int fd, struct sockaddr_in *addr);

// core command functions
void register_command(char *message, int fd, struct sockaddr_in *addr);
void unregister_command(char *message, int fd, struct sockaddr_in *addr);
void login_command(char *message, int fd, struct sockaddr_in *addr);
void logout_command(char *message, int fd, struct sockaddr_in *addr);
void groups_command(char *message, int fd, struct sockaddr_in *addr);
void subscribe_command(char *message, int fd, struct sockaddr_in *addr);
void unsubscribe_command(char *message, int fd, struct sockaddr_in *addr);
void my_groups_command(char *message, int fd, struct sockaddr_in *addr);
void ulist_command(int fd, struct sockaddr_in *addr);
void post_command(int fd, struct sockaddr_in *addr);
void retrieve_command(int fd, struct sockaddr_in *addr);

// command message processing functions 
void process_register_message(char *message, char *reply);
void process_unregister_message(char *message, char *reply);
void process_login_message(char *message, char *reply);
void process_logout_message(char *message, char *reply);
void process_groups_message(char *message, char *reply);
void process_subscribe_message(char *message, char *reply);
void process_unsubscribe_message(char *message, char *reply);
void process_my_groups_message(char *message, char *reply);
void process_ulist_message(char *buffer, char *reply);

// message validation functions
int validate_post_message(char *UID, char *GID, char *Tsize, char *text);
int validate_retrieve_message(char *UID, char *GID, char *MID);

// user related functions
int user_is_registered(char *UID);
int user_is_logged_in(char *UID);
int user_is_subscribed_to_group(char *UID, char *GID);
int register_user(char *UID, char *pass);
int unregister_user(char *UID, char *pass);
int login_user(char *UID, char *pass);
int logout_user(char *UID, char *pass);
int subscribe_user(char *UID, char *GID);
int unsubscribe_user(char *UID, char *GID);
int check_password(char *pass, char *user_pass_path);

// group related functions
int create_new_group(char *GID, char *GName);
int group_exists(char *GID);
int group_has_messages(char *GID, char *MID);

// GROUPLIST related functions
int   get_index(GROUPLIST *list, char *GID);
int   get_number_of_messages(char *GID, char *MID);
int   get_groups(GROUPLIST *list);
int   get_my_groups(GROUPLIST *list, char *UID);
int   get_users_of_group(char *user_list, char *GID, char *GName);
int   get_group_name(char *GID, char *GName);
char *GROUPLIST_to_string(GROUPLIST *list);
void  initialize_group_list(GROUPLIST *list);
void  SortGList(GROUPLIST *list);
void  swap_groups(int g1, int g2, GROUPLIST *list);
void  increment_last_message_available(GROUPLIST *list, char *GID);
void  get_next_MID(char *MID, GROUPLIST *list, char *GID);
void  increment_MID(char *MID);
void  update_last_available_message(GROUPLIST *list, char *GID, int i);
void  convert_MID_int_to_string(int MID_int, char *MID);

// socket UDP related functions
int  create_socket_UDP();
void send_reply_UDP(char *reply, int fd, struct sockaddr_in *addr);
void receive_message_UDP(int fd, char *message, struct sockaddr_in *addr);
void get_address_info_UDP(struct addrinfo *hints, struct addrinfo **res, char *port);

// socket TCP related funtions
int  create_socket_TCP();
int  receive_n_plus_1_chars_TCP(int n, char *string, int fd); 
void get_address_info_TCP(struct addrinfo *hints, struct addrinfo **res, char *port);
void receive_n_chars_TCP(int n, char *string, int fd); 
void receive_n_tokens_TCP(int n, char *string, int fd); 
void receive_data_TCP(char *FName, char *Fsize, char *GID, char *MID, int fd);
void send_n_chars_TCP(int n, char *string, int fd); 
void send_TCP(char *string, int fd); 
void send_data_TCP(char *FName, int fd); 
void retrieve_and_send_messages_TCP(char *UID, char *GID, char *MID, int fd); 

// socket UDP and TCP related functions
void get_user_ip_and_port(int fd, char *user_ip, char *user_port, struct sockaddr_in *addr);

// file processing related functions
int  get_number_of_files(char *dir_path);
void make_file(char *file_path);
void make_author_file(char *UID, char *GID,char *MID);
void make_text_file(char *text, char *GID, char *MID);
void delete_file(char *file_path);
void read_text_from_file(char *text, char *file_path, int size);
void get_UID_from_file_name(char *UID, char *file_name);

#endif