#include <stdio.h>
#include "http.h"
#include "server.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage of %s: ./%s <Port Number>", argv[0], argv[0]);
        return 1;
    }

    int listen_fd = create_server_socket(argv[1]);

    while(1) {
        
    }

    return 0;
}
