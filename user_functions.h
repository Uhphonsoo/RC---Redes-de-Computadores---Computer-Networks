#ifndef _USER_FUNCTIONS_
#define _USER_FUNCTIONS_

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

void  validate_program_input(int, char**);
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

#endif