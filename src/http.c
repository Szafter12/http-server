#include "http.h"

void proccess_request(const int conn_fd) {
  char method[MAX_LINE], path[MAX_LINE], http_ver[MAX_LINE], content_type[MAX_LINE], connection[MAX_LINE], body[MAX_BODY];
  size_t content_length = 0;
  rio_t rp;

  rio_readinitb(&rp, conn_fd);

  read_header(&rp, method, path, http_ver, content_type, connection, body, content_length);

  generate_response(conn_fd, path);
}

void generate_response(int conn_fd, char *path) {
  struct stat sbuf;
  char file_name[MAX_LINE];
  char file_mime[MAX_LINE];
  char file_ext[MAX_LINE];
  size_t file_size = 0;
  int file_fd;

  if (strcmp(path, "/") == 0) {
    strncat(path, "index.html", MAX_LINE);
  }
  snprintf(file_name, sizeof(file_name), ".%s", path);

  if (stat(file_name, &sbuf) < 0) {
    //send_error(conn_fd, "404 Not Found", "File didn't exist");
    return;
  }

  if (!(S_ISREG(sbuf.st_mode)) || !(sbuf.st_mode & S_IRUSR)) {
    //send_error(conn_fd, "403 Forbidden", "You don't have access to this resource");
    return;
  }

  if (stat(file_name, &sbuf) == 0) {
    if (sbuf.st_mode & S_IXUSR) {
      //proccess_dynamic_content();
    } else {
      get_ext(file_name, file_ext);
      get_content_type(file_ext, file_mime);
      file_size = sbuf.st_size;
      file_fd = open(file_name, O_RDONLY, 0);

      send_header(conn_fd, file_size, file_mime);
      send_content(file_fd, conn_fd);

      close(file_fd);
    }
  }
}

void send_header(const int conn_fd, size_t file_size, char *file_mime) {
  char header[MAX_BUFF];

  snprintf(header, sizeof(header),
     "HTTP/1.1 200 OK\r\n"
     "Server: CustomServer\r\n"
     "Connection: close\r\n"
     "Content-Length: %zu\r\n"
     "Content-Type: %s\r\n\r\n",
     file_size, file_mime);

  rio_writen(conn_fd, header, strlen(header));
}

void send_content(int file_fd, int conn_fd) {
  char buf[MAX_BUFF];
  ssize_t n;

  while ((n = read(file_fd, buf, sizeof(buf))) > 0) {
    rio_writen(conn_fd, buf, n);
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

void read_header(const rio_t *rp, char *method, char *path, char *http_ver, char *content_type, char *connection, char *body, size_t content_length) {
  char buff[MAX_BUFF];

  rio_readlineb(rp, buff, MAX_BUFF);
  sscanf(buff, "%s %s %s", method, path, http_ver);

  while (rio_readlineb(rp, buff, MAX_LINE) > 0) {
    if (strcmp(buff, "\r\n") == 0) break;

    if (strncasecmp(buff, "Content-Type:", 13) == 0) sscanf(buff, "%*s %s", content_type);

    if (strncasecmp(buff, "Connection:", 11) == 0) sscanf(buff, "%*s %s", connection);

    if (strncasecmp(buff, "Content-Length:", 15) == 0) sscanf(buff, "%*s %zu", &content_length);
  }

  if (content_length > 0 && content_length < MAX_BODY) {
    rio_readnb(rp, body, content_length);
    body[content_length] = '\0';
  }
}

