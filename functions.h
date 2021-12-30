#ifndef _FUNCTIONS_
#define _FUNCTIONS_

#define max(A,B) ((A)>=(B)?(A):(B))

// misc
void  validate_sendto(int);
void  validate_recvfrom(int);
void  validate_connect(int);
void  validate_write(int);
void  validate_read(int);
void  validate_accept(int fd);
void  validate_bind(int n);
void validate_getaddrinfo(int errcode);
int   validate_UID(char*);
int   validate_pass(char*);
int   validate_GID(char*);
int   validate_GName(char*);
int   validate_MID(char*);
void  get_first_token(char*, char*);
int   get_nth_token(char*, int, char*);
int   get_number_of_tokens(char*);
int   get_next_token(char*, int, char*);
int   get_file_size(FILE*); 
void  show_groups(char*, char*);
void  show_users(char*);
void  show_messages(char*);
int   is_empty_string(char*);
void  clear_string(char*);
void  terminate_string_after_n_tokens(char*, int);
void  clear_message_and_reply();
void  close_TCP_connections();

#endif