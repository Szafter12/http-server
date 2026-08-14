#ifndef HTTP
#define HTTP

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rio.h"
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_BODY 8192
#define MAX_BUFF 4096

typedef struct {
    const char *ext;
    const char *mime;
} mime_map_t;

void proccess_request(const int conn_fd);
void get_content_type(const char *file_name, char *file_mime);
void get_ext(const char *file_name, char *file_ext);
void read_header(const rio_t *rp, char *method, char *path, char *http_ver, char *content_type, char *connection, char *body, size_t content_length);
void generate_response(const int conn_fd, char *path);
// void proccess_static_content();
void send_content(const int file_fd, int conn_fd);
void send_header(const int conn_fd, size_t file_name, char *file_mime);

#endif
