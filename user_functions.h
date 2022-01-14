#ifndef _USER_FUNCTIONS_
#define _USER_FUNCTIONS_

#include <stdio.h>

// registration commands
void register_command(char *command);
void unregister_command(char *command);

// user identification and session termination commands
void login_command(char *command);
void logout_command(char *command);
void showuid_command();
void exit_command(char *command);

// group management commads
void groups_command(char *command);
void subscribe_command(char *command);
void unsubscribe_command(char *command);
void my_groups_command(char *command);
void select_command(char *command);
void showgid_command();
void ulist_command();

// messaging commands
void post_command(char *command);
void retrieve_command(char *command);

void validate_program_input(int argc, char **argv);
int  validate_registration_command(char *command, char *UID, char *pass);
int  validate_login_command(char *command, char *UID, char *pass);
int  validate_logout_command(char *command);
int  validate_exit_command(char *command);
int  validate_groups_command(char *command);
int  validate_subscribe_command(char *commad, char *GID, char *GName);
int  validate_unsubscribe_command(char *command, char *GID);
int  validate_my_groups_command(char *command);
int  validate_select_command(char *command, char *GID);
int  validate_post_command(char *command, char *aux, char *text, char *Fname, int *file_is_being_sent);
int  validate_retrieve_command(char *command, char *MID);
void validate_register_reply(char *reply, char *aux, char *status);
void validate_unregister_reply(char *reply,char *aux, char *status);
void validate_login_reply(char *reply, char *aux, char *status);
void validate_logout_reply(char *reply, char *aux, char *status);
void validate_groups_reply(char *reply, char *aux, char *N);
void validate_subscribe_reply(char *reply, char *aux, char *status);
void validate_usubscribe_reply(char *reply, char *aux, char *status);
void validate_my_groups_reply(char *reply, char *aux, char *N);
int  validate_ulist_reply(char *reply, char *aux, char *status);
void validate_post_reply(char *reply, char *aux, char *status);
int  validate_retrieve_reply(char *reply, char *aux, char *status, char *N);

// socket related functions
void create_socket_UDP();
void get_address_info_UDP();
void create_socket_TCP();
void get_address_info_TCP();
void send_and_receive_UDP(char *message, char *reply);
void send_and_receive_TCP(char *message, char *reply);
void send_TCP(char *string);
void send_data_TCP(FILE *fp);
void receive_TCP(char *string);
void receive_n_tokens_TCP(int n, char *string);
void receive_n_chars_TCP(int n, char *string);
void receive_n_bytes_TCP(int n, char *string);
void receive_data_TCP(char *file_path, char *Fsize);

// error and message related functions
void  print_program_input_error();
void  print_invalid_UID_message();
void  print_invalid_pass_message();
void  print_invalid_number_of_arguments_message();
void  print_invalid_GID_message();
void  print_invalid_GName_message();
void  print_invalid_MID_message();

#endif