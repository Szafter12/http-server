#include "http.h"
#include "server.h"
#include "threadpool.h"

/*
 * TODO
 * - Add simple CGI
 * - Implement multithreading
 */

int main(int argc, char *argv[]) {
    char host[NI_MAXHOST] = {0}, port[NI_MAXSERV] = {0};
    socklen_t client_len;
    struct sockaddr_storage client_addr;
    int flags = NI_NUMERICHOST | NI_NUMERICSERV;

    if (argc != 2) {
        fprintf(stderr, "Usage of %s: ./%s <Port Number>\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    int listen_fd = create_server_socket(argv[1]);
    pthread_t tid;
    printf("Listen on port %s\n", argv[1]);

    while(1) {
        client_len = sizeof(client_addr);
        int *conn_fd_p = (int *)malloc(sizeof(int));
        *conn_fd_p = accept(listen_fd, (SA *) &client_addr, &client_len);

        int rc;
        if ((rc = getnameinfo((SA *)&client_addr, client_len, host, NI_MAXHOST, port, NI_MAXSERV, flags)) != 0) {
            fprintf(stderr, "Error while getnameinfo: %s\n", gai_strerror(rc));
            exit(EXIT_FAILURE);
        }

        if (*conn_fd_p < 0) {
            free(conn_fd_p);
            continue;
        }

        printf("A connection was successfully established with %s on port %s\n", host, port);

        pthread_create(&tid, NULL, thread, conn_fd_p);
    }

    close(listen_fd);
    return 0;
}
