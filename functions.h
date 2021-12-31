#ifndef _FUNCTIONS_
#define _FUNCTIONS_

/* #define max(A,B) ((A)>=(B)?(A):(B)) */

// misc
void  validate_sendto(int n);
void  validate_recvfrom(int n);
void  validate_connect(int n);
void  validate_write(int n);
void  validate_read(int n);
void  validate_accept(int fd);
void  validate_bind(int n);
void  validate_getaddrinfo(int errcode);
int   validate_UID(char *UID);
int   validate_pass(char *pass);
int   validate_GID(char *GID);
int   validate_GName(char *GName);
int   validate_MID(char *MID);
void  get_first_token(char *string, char *first_token);
int   get_nth_token(char *string, int n, char *token);
int   get_number_of_tokens(char *string);
int   get_next_token(char *string, int position, char *token);
int   get_file_size(FILE*); 
void  show_groups(char *reply, char *N);
void  show_users(char *reply);
void  show_messages(char *reply);
int   is_empty_string(char *string);
void  clear_string(char *string);
void  terminate_string_after_n_tokens(char *string, int n);
void  clear_message_and_reply(char *message_, char *reply_);

#endif