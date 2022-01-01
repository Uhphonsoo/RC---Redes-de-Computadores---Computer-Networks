#ifndef _SERVER_FUNCTIONS_
#define _SERVER_FUNCTIONS_

void validate_program_input(int argc, char **argv, char *DSport);

void process_message(char *message, int fd, struct sockaddr_in *addr);

void register_command(char *message, int fd, struct sockaddr_in *addr);
void unregister_command(char *message, int fd, struct sockaddr_in *addr);
void login_command(char *message, int fd, struct sockaddr_in *addr);
void logout_command(char *message);
void groups_command(char *message);
void subscribe_command(char *message);
void unsubscribe_command(char *message);
void my_groups_command(char *message);
void ulist_command(char *message);
void post_command(char *message);
void retrieve_command(char *message);

void process_register_message(char *message, char *reply);
void process_unregister_message(char *message, char *reply);
void process_login_message(char *message, char *reply);

int user_is_registered(char *UID);
int register_user(char *UID, char *pass);
int unregister_user(char *UID, char *pass);
int login_user(char *UID, char *pass);

int check_password(char *pass, char *user_pass_path);

int  create_socket_datagram();
void get_address_info_datagram(struct addrinfo *hints, struct addrinfo **res, char *port);
int  create_socket_stream();
void get_address_info_stream(struct addrinfo *hints, struct addrinfo **res, char *port);
// void create_server_socket_UDP();
// void get_address_info_server_UDP();
// void create_server_socket_TCP();
// void get_address_info_server_TCP();
void receive_message_UDP(int fd, char *message, struct sockaddr_in *addr);
void receive_message_TCP(int fd, char *message);
void send_reply_UDP(char *reply, int fd, struct sockaddr_in *addr);

void make_file(char *file_path);
void delete_file(char *file_path);


void get_client_ip_and_port(int fd, char *client_ip, char *client_port, struct sockaddr_in *addr);

#endif