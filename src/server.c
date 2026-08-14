#include "server.h"

int create_server_socket(char *port_number) {
  struct addrinfo *p, *result_list, hints;
  int socked_fd, opt = 1;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;

  int rc;
  if ((rc = getaddrinfo(NULL, port_number, &hints, &result_list)) != 0) {
    fprintf(stderr, "Error occurs in getaddrinfo: %s", gai_strerror(rc));
    exit(EXIT_FAILURE);
  }

  for (p = result_list; p; p = p->ai_next) {
    if ((socked_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
      continue;

    if (socked_fd < 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }

    if (setsockopt(socked_fd, SOL_SOCKET, SO_REUSEPORT,(void *)&opt, sizeof(opt))) {
      perror("setsockopt failed");
      exit(EXIT_FAILURE);
    }

    if (bind(socked_fd, p->ai_addr, p->ai_addrlen) == 0) break;

    perror("bind failed");
    close(socked_fd);
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(result_list);
  result_list = NULL;

  if (listen(socked_fd, BACKLOG) != 0) {
    perror("Listen error");
    close(socked_fd);
    exit(EXIT_FAILURE);
  }

  return socked_fd;
}
