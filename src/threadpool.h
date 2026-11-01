#include <pthread.h>
#include "http.h"

void *thread(void *vargp) {
    pthread_detach(pthread_self());

    int connfd = *((int *)vargp);
    free(vargp);

    proccess_request(connfd);
    close(connfd);

    return NULL;
}