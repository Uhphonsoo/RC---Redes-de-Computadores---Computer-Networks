#ifndef _SERVER_FUNCTIONS_
#define _SERVER_FUNCTIONS_

int  create_socket();
void get_address_info(struct addrinfo *hints, struct addrinfo **res);
void create_server_socket_UDP();
void get_address_info_server_UDP();
void create_server_socket_TCP();
void get_address_info_server_TCP();

#endif