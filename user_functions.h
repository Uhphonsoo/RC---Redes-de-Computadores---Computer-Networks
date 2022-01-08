#ifndef _USER_FUNCTIONS_
#define _USER_FUNCTIONS_

#include <stdio.h>

// client functions
// registration commands
void register_command(char*);
void unregister_command(char*);

// user identification and session termination commands
void login_command(char*);
void logout_command(char*);
void showuid_command();
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

void  validate_program_input(int argc, char **argv);
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

// socket related functions
void create_UDP_socket();
void get_address_info_UDP();
void create_TCP_socket();
void get_address_info_TCP();
void send_and_receive_UDP(char*, char*);
void send_and_receive_TCP(char *message, char *reply, int); // ccc
void send_TCP(char *string); // ccc
void send_data_TCP(FILE *fp, int Fsize); // ccc
void receive_TCP(char *string); // ccc
// void receive_first_tokens_retrieve_TCP(char *string); // !!!
void receive_n_tokens_TCP(int n, char *string); // ccc
void receive_n_chars_TCP(int n, char *string); // ccc
void receive_n_bytes_TCP(int n, char *string); // ccc
void receive_data_TCP(char *file_path, char *Fsize); // ccc

void  input_error();
void  invalid_UID();
void  invalid_Upass();
void  invalid_NArgs();
void  invalid_GID();
void  invalid_GName();
void  invalid_MID();

#endif