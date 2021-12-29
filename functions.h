#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_
#include "functions.h"

// registration commands
void register_command(char*);
void unregister_command(char*);

// user identification and session termination commands
void login_command(char*);
void logout_command(char*);
void showuid_command(char*);
void exit_command(char*);

// group management commads
void groups_command(char*);
void subscribe_command(char*);
void unsubscribe_command(char*);
void my_groups_command(char*);
void select_command(char*);
void showgid_command();
void ulist_command();

// messaging commands
void post_command(char*);
void retrieve_command(char*);

// misc
int   validate_registration_command(char*, char*, char*);
int   validate_login_command(char*, char*, char*);
int   validate_logout_command(char*);
int   validate_exit_command(char*);
int   validate_groups_command(char*);
int   validate_subscribe_command(char*, char*, char*);
int   validate_unsubscribe_command(char*, char*);
int   validate_my_groups_command(char*);
int   validate_select_command(char*, char*);
int   validate_post_command(char*, char*, char*, char*, int*);
int   validate_retrieve_command(char*, char*);
void  validate_register_reply(char*, char*, char*);
void  validate_unregister_reply(char*,char*, char*);
void  validate_login_reply(char*, char*, char*);
void  validate_logout_reply(char*, char*, char*);
void  validate_groups_reply(char*, char*, char*);
void  validate_subscribe_reply(char*, char*, char*);
void  validate_usubscribe_reply(char*, char*, char*);
void  validate_my_groups_reply(char*, char*, char*);
int   validate_ulist_reply(char*, char*, char*);
void  validate_post_reply(char*, char*, char*);
int   validate_retrieve_reply(char*, char*, char*, char*);
void  validate_program_input(int, char**);
void  validate_sendto(int);
void  validate_recvfrom(int);
void  validate_connect(int);
void  validate_write(int);
void  validate_read(int);
int   validate_UID(char*);
int   validate_pass(char*);
int   validate_GID(char*);
int   validate_GName(char*);
int   validate_MID(char*);
void  create_UDP_socket();
void  get_address_info_UDP();
void  create_TCP_socket();
void  get_address_info_TCP();
void  get_first_token(char*, char*);
int   get_nth_token(char*, int, char*);
int   get_number_of_tokens(char*);
int   get_next_token(char*, int, char*);
int   get_file_size(FILE*); 
void  send_and_receive_UDP(char*, char*);
void  send_and_receive_TCP(char*, char*, int);
void  show_groups(char*, char*);
void  show_users(char*);
void  show_messages(char*);
int   is_empty_string(char*);
void  clear_string(char*);
void  terminate_string_after_n_tokens(char*, int);
void  close_TCP_connections();

#endif