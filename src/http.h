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

typedef struct {
    char method[MAX_LINE],
    path[MAX_LINE],
    query_string[MAX_LINE],
    http_ver[MAX_LINE],
    content_type[MAX_LINE],
    connection[MAX_LINE],
    body[MAX_BODY];
    size_t content_length;
} req_info_t;

typedef struct {
    char http_ver[MAX_LINE],
    status_code[MAX_LINE],
    Location[MAX_LINE],
    content_type[MAX_LINE],
    body[MAX_BODY];
    size_t content_length;
} res_info_t;

typedef struct {
    char file_path[MAX_LINE];
    char mime[MAX_LINE];
    char file_ext[MAX_LINE];
    size_t file_size;
} file_handle_t;

// Main functions
void proccess_request(int conn_fd);
void generate_response(int conn_fd, req_info_t *req);
void read_header(int conn_fd,rio_t *rp, req_info_t *req);



// Static content functions
void proccess_static_content(int conn_fd, file_handle_t *fh, const struct stat *sbuf, const char *method);
void send_static_content(int file_fd, int conn_fd);
void send_static_header(int conn_fd, size_t file_name, char *file_mime);

// helper functions
void get_content_type(const char *file_name, char *file_mime);
void get_ext(const char *file_name, char *file_ext);
void send_error(int conn_fd, char *err_code, char *err_msg);
int parse_file_path(char *path, char *file_path, char *query_string);

#endif
