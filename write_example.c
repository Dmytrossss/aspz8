#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    int flags = fcntl(pipefd[1], F_GETFL);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(1);
    }
    if (fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(1);
    }

    size_t nbytes = 1000000;
    char *buffer = (char *)malloc(nbytes);
    if (!buffer) {
        perror("malloc");
        exit(1);
    }
    memset(buffer, 'A', nbytes);

    ssize_t count = write(pipefd[1], buffer, nbytes);
    if (count < 0) {
        perror("write");
        printf("Помилка: %s\n", strerror(errno));
    } else {
        printf("Записано: %zd з %zu байт\n", count, nbytes);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    free(buffer);
    return 0;
}
