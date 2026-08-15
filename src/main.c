#include "http.h"
#include "server.h"

/*
 * TODO
 * - Implement uri path parser to avoid path with query parameters (/path?somequery)
 * - Impelement protection to access resources out web root dir
 * - Refactor http.c functions for more robust implementation
 * - Add error more secure error handling to http.c functions
 * - Add PHP CGI implementation to parse POST and GET methods with dynamic content
 * - Implement multithreading
 */

int main(int argc, char *argv[]) {
    int listen_fd, conn_fd;
    char host[NI_MAXHOST], port[NI_MAXSERV];
    socklen_t client_len;
    struct sockaddr_storage client_addr;
    int flags = NI_NUMERICHOST | NI_NUMERICSERV;

    if (argc != 2) {
        fprintf(stderr, "Usage of %s: ./%s <Port Number>\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    listen_fd = create_server_socket(argv[1]);

    printf("Listen on port %s\n", argv[1]);

    while(1) {
        client_len = sizeof(client_addr);
        conn_fd = accept(listen_fd, (SA *)&client_addr, &client_len);

        int rc;
        if ((rc = getnameinfo((SA *)&client_addr, client_len, host, NI_MAXHOST, port, NI_MAXSERV, flags)) != 0) {
            fprintf(stderr, "Error while getnameinfo: %s\n", gai_strerror(rc));
            exit(EXIT_FAILURE);
        }

        printf("A connection was successfully established with %s on port %s\n", host, port);
        proccess_request(conn_fd);
        close(conn_fd);
    }

    return 0;
}
