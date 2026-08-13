#ifndef SERVER
#define SERVER

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int create_server_socket(char *port_number);

#endif
