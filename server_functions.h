#ifndef _SERVER_FUNCTIONS_
#define _SERVER_FUNCTIONS_

void validate_program_input(int argc, char **argv, char *DSport);
int  create_socket_datagram();
void get_address_info_datagram(struct addrinfo *hints, struct addrinfo **res, char *port);
int  create_socket_stream();
void get_address_info_stream(struct addrinfo *hints, struct addrinfo **res, char *port);
// void create_server_socket_UDP();
// void get_address_info_server_UDP();
// void create_server_socket_TCP();
// void get_address_info_server_TCP();
void receive_message_UDP(int fd, char *message);
void receive_message_TCP(int fd, char *message);
void process_message(char *message, int fd);

void register_command(char *message, int fd);
void unregister_command(char *message);
void login_command(char *message);
void logout_command(char *message);
void groups_command(char *message);
void subscribe_command(char *message);
void unsubscribe_command(char *message);
void my_groups_command(char *message);
void ulist_command(char *message);
void post_command(char *message);
void retrieve_command(char *message);

void validate_register_message(char *message, char *status);

void send_reply_UDP(char *reply, int fd);

#endif