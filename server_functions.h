#ifndef _SERVER_FUNCTIONS_
#define _SERVER_FUNCTIONS_

int  create_socket_datagram();
void get_address_info_datagram(struct addrinfo *hints, struct addrinfo **res, char *port);
int  create_socket_stream();
void get_address_info_stream(struct addrinfo *hints, struct addrinfo **res, char *port);
void create_server_socket_UDP();
void get_address_info_server_UDP();
void create_server_socket_TCP();
void get_address_info_server_TCP();

#endif