#include "http.h"

#define MAX_LINE 1024
#define MAX_BODY 8192

void proccess_request(int conn_fd) {
  char method[MAX_LINE], path[MAX_LINE], http_ver[MAX_LINE], content_type[MAX_LINE], connection[MAX_LINE], body[MAX_BODY];
  int content_length;
  rio_t rio;

  
}
