#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("data.bin", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    unsigned char data[] = {4, 5, 2, 2, 3, 3, 7, 9, 1, 5};
    ssize_t n = write(fd, data, sizeof(data));
    if (n != sizeof(data)) {
        perror("write");
        close(fd);
        return 1;
    }

    if (lseek(fd, 3, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }

    unsigned char buffer[4];
    n = read(fd, buffer, 4);
    if (n != 4) {
        perror("read");
        close(fd);
        return 1;
    }

    printf("Буфер: %d, %d, %d, %d\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    close(fd);
    return 0;
}
