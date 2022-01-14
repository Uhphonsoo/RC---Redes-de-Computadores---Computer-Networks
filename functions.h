#ifndef _FUNCTIONS_
#define _FUNCTIONS_

// system call validation functions
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
void validate_socket(int ret);

// other validation functions
int validate_UID(char *UID);
int validate_pass(char *pass);
int validate_GID(char *GID);
int validate_GName(char *GName);
int validate_MID(char *MID);

// printing functions
void show_groups(char *reply, char *N);
void show_users(char *reply);
void show_messages(char *reply);

// string manipulation functions
int  get_nth_token(char *string, int n, char *token);
int  get_number_of_tokens(char *string);
int  get_next_token(char *string, int position, char *token);
int  is_empty_string(char *string);
void get_first_token(char *string, char *first_token);
void clear_string(char *string);
void terminate_string_after_n_tokens(char *string, int n);
void strip_quotes_from_string(char *string);

// file manipulation functions
int  get_file_size(FILE *fp); 
int  get_file_size_char(char *file_path);

#endif