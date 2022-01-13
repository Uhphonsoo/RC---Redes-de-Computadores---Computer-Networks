#ifndef _FUNCTIONS_
#define _FUNCTIONS_

/* #define max(A,B) ((A)>=(B)?(A):(B)) */

// system call validations
void validate_sendto(int ret);
void validate_recvfrom(int ret);
void validate_connect(int ret);
void validate_write(int ret);
void validate_read(int ret);
void validate_accept(int fd);
void validate_bind(int ret);
void validate_getaddrinfo(int errcode);
void validate_select(int ret);
void validate_fopen(FILE *fp);
void validate_fclose(int ret);
void validate_fprintf(int ret);
void validate_mkdir(int ret);
void validate_listen(int ret);

// other validations
int validate_UID(char *UID);
int validate_pass(char *pass);
int validate_GID(char *GID);
int validate_GName(char *GName);
int validate_MID(char *MID);

void  get_first_token(char *string, char *first_token);
int   get_nth_token(char *string, int n, char *token);
int   get_number_of_tokens(char *string);
int   get_next_token(char *string, int position, char *token);
long  get_file_size(FILE *fp); 
long  get_file_size_char(char *file_path);
void  show_groups(char *reply, char *N);
void  show_users(char *reply);
void  show_messages(char *reply);
int   is_empty_string(char *string);
void  clear_string(char *string);
void  terminate_string_after_n_tokens(char *string, int n);
void  clear_message_and_reply(char *message_, char *reply_);
void  strip_quotes_from_string(char *string);

#endif