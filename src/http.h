#ifndef HTTP
#define HTTP

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
    const char *ext;
    const char *mime;
} mime_map_t;

void proccess_request(int conn_fd);
void get_content_type(const char *file_name, char *file_mime);
void get_ext(const char *file_name, char *file_ext);

#endif
