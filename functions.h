#ifndef _FUNCTIONS_
#define _FUNCTIONS_

#define max(A,B) ((A)>=(B)?(A):(B))

// misc
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
void  clear_message_and_reply();
void  close_TCP_connections();

#endif