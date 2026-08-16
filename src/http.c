#include "http.h"

void proccess_request(const int conn_fd) {
  req_info_t *req = malloc(sizeof(req_info_t));
  if (req == NULL) {
    fprintf(stderr, "malloc() failed\n");
    free(req);
    return;
  }

  rio_t rp;
  rio_readinitb(&rp, conn_fd);

  read_header(conn_fd, &rp, req);
  generate_response(conn_fd, req);

  free(req);
}

int parse_file_path(char *path, char *file_path, char *query_string) {
  if (strstr(path, "..") != NULL) {
    return -1;
  }

  char *ptr = strchr(path, '?');
  if (ptr != NULL) {
    *ptr = '\0';
    snprintf(query_string, MAX_LINE, "%s", ++ptr);
  } else {
    query_string[0] = '\0';
  }

  if (strcmp(path, "/") == 0) {
    snprintf(file_path, MAX_LINE, "./index.html");
  } else {
    snprintf(file_path, MAX_LINE, ".%s", path);
  }

  return 0;
}

void generate_response(int conn_fd, req_info_t *req) {
  struct stat sbuf;

  file_handle_t *fh = malloc(sizeof(file_handle_t));
  if (fh == NULL) {
    fprintf(stderr, "malloc() failed\n");
    return;
  }

  res_info_t *res_info = malloc(sizeof(res_info_t));
  if (res_info == NULL) {
    fprintf(stderr, "malloc() failed\n");
    return;
  }

  int status = parse_file_path(req->path, fh->file_path, req->query_string);

  if (stat(fh->file_path, &sbuf) < 0) {
    send_error(conn_fd, "404 Not Found", "File didn't exist");
    free(res_info);
    free(fh);
    return;
  }

  if (!(S_ISREG(sbuf.st_mode)) || !(sbuf.st_mode & S_IRUSR) || status == 1) {
    send_error(conn_fd, "403 Forbidden", "You don't have access to this resource");
    free(res_info);
    free(fh);
    return;
  }

  if (sbuf.st_mode & S_IXUSR) {
    //proccess_dynamic_content();
  } else {
    get_ext(fh->file_path, fh->file_ext);
    get_content_type(fh->file_ext, fh->mime);
    fh->file_size = sbuf.st_size;
    int file_fd = open(fh->file_path, O_RDONLY, 0);

    send_header(conn_fd, fh->file_size, fh->mime);
    send_content(file_fd, conn_fd);

    close(file_fd);
  }

  free(res_info);
  free(fh);
}

void send_header(int conn_fd, size_t file_size, char *file_mime) {
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

void read_header(int conn_fd,rio_t *rp, req_info_t *req) {
  char buff[MAX_BUFF];
  int rc = 0;

  rio_readlineb(rp, buff, MAX_BUFF);

  rc = sscanf(buff, "%255s %255s %255s", req->method, req->path, req->http_ver);
  if (rc != 3) {
    send_error(conn_fd, "400 Bad request", "Invalid http request");
  }

  while (rio_readlineb(rp, buff, MAX_LINE) > 0) {
    if (strcmp(buff, "\r\n") == 0) break;

    if (strncasecmp(buff, "Content-Type:", 13) == 0) sscanf(buff, "%*s %s", req->content_type);

    if (strncasecmp(buff, "Connection:", 11) == 0) sscanf(buff, "%*s %s", req->connection);

    if (strncasecmp(buff, "Content-Length:", 15) == 0) sscanf(buff, "%*s %zu", &req->content_length);
  }

  if (req->content_length > 0 && req->content_length < MAX_BODY && strcmp(req->method, "GET") != 0) {
    rio_readnb(rp, req->body, req->content_length);
    req->body[req->content_length] = '\0';
  }
}

void send_error(int conn_fd, char *err_code, char *err_msg) {
  char header[MAX_BUFF];
  char body[MAX_BODY];

  snprintf(body, MAX_BODY,
    "<html><title>Error</title>"
    "<body bgcolor=""ffffff"">\r\n"
    "<p>%s</p>"
    "</body></html>",
    err_msg);

  snprintf(header, sizeof(header),
     "HTTP/1.1 %s\r\n"
     "Server: CustomServer\r\n"
     "Connection: close\r\n"
     "Content-Type: text/html; charset=utf-8\r\n"
     "Content-Length: %zu\r\n\r\n",
     err_code, (unsigned long)strlen(body));

  rio_writen(conn_fd, header, strlen(header));
  rio_writen(conn_fd, body, strlen(body));
}