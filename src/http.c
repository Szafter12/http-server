#include "http.h"

#include <unistd.h>

#include "rio.h"

#define MAX_LINE 1024
#define MAX_BODY 8192
#define MAX_BUFF 4096

void proccess_request(int conn_fd) {
  char buff[MAX_BUFF];
  char method[MAX_LINE], path[MAX_LINE], http_ver[MAX_LINE], content_type[MAX_LINE], connection[MAX_LINE], body[MAX_BODY];
  int content_length = 0;
  rio_t rp;
  
  rio_readinitb(&rp, conn_fd);
  rio_readlineb(&rp, buff, MAX_BUFF);
  
  sscanf(buff, "%s %s %s", method, path, http_ver);
  while (rio_readlineb(&rp, buff, MAX_LINE) > 0) {
    if (strcmp(buff, "\r\n") == 0) break;

    if (strncasecmp(buff, "Content-Type:", 13) == 0) sscanf(buff, "%*s %s", content_type);

    if (strncasecmp(buff, "Connection:", 11) == 0) sscanf(buff, "%*s %s", connection);

    if (strncasecmp(buff, "Content-Length:", 15) == 0) sscanf(buff, "%*s %d", &content_length);
  }

  if (content_length > 0 && content_length < MAX_BODY) {
    rio_readnb(&rp, body, content_length);
    body[content_length] = '\0';
  }

  struct stat sbuf;
  if (strcmp(path, "/") == 0) strncat(path, "index.html", MAX_LINE);
  char file_name[MAX_LINE];
  snprintf(file_name, sizeof(file_name), ".%s", path);

  char file_mime[MAX_LINE];
  char file_ext[MAX_LINE];
  size_t file_size = 0;

  if (stat(file_name, &sbuf) == 0) {
    if (sbuf.st_mode & S_IXUSR) {

    } else {
      get_ext(file_name, file_ext);
      get_content_type(file_ext, file_mime);

      file_size = sbuf.st_size;

      int filefd = open(file_name, O_RDONLY, 0);

      char header[MAX_BUFF];

      snprintf(header, sizeof(header),
         "HTTP/1.1 200 OK\r\n"
         "Server: CustomServer\r\n"
         "Connection: close\r\n"
         "Content-Length: %zu\r\n"
         "Content-Type: %s\r\n\r\n",
         file_size, file_mime);

      rio_writen(conn_fd, header, strlen(header));

      char buf[MAX_BUFF];
      ssize_t n;

      while ((n = read(filefd, buf, sizeof(buf))) > 0) {
        rio_writen(conn_fd, buf, n);
      }
      close(filefd);
    }
  }
}

void get_ext(const char *file_name, char *file_ext) {
  const char *dot = strrchr(file_name, '.');

  if (!dot || dot == file_name) {
    strncpy(file_ext, "", MAX_LINE);
    return;
  }

  strncpy(file_ext, dot, MAX_LINE);
}

void get_content_type(const char *ext, char *file_mime) {
  static const mime_map_t MIME_TYPES[] = {
    {".html", "text/html; charset=utf-8"},
    {".htm",  "text/html; charset=utf-8"},
    {".css",  "text/css; charset=utf-8"},
    {".js",   "application/javascript; charset=utf-8"},
    {".json", "application/json; charset=utf-8"},
    {".png",  "image/png"},
    {".jpg",  "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif",  "image/gif"},
    {".svg",  "image/svg+xml"},
    {".ico",  "image/x-icon"},
    {".pdf",  "application/pdf"},
    {".txt",  "text/plain; charset=utf-8"},
    {NULL,    "application/octet-stream"}
  };

  if (ext == NULL || *ext == '\0') {
    strncpy(file_mime, "application/octet-stream", MAX_LINE);
    return;
  }

  for (int i = 0; MIME_TYPES[i].ext != NULL; i++) {
    if (strcasecmp(ext, MIME_TYPES[i].ext) == 0) {
      strncpy(file_mime, MIME_TYPES[i].mime, MAX_LINE);
      return;
    }
  }

  strncpy(file_mime, "application/octet-stream", MAX_LINE);
}


