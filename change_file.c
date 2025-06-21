#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *text = "Hello, World!";
    ssize_t written = write(fd, text, strlen(text));
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    }

    off_t pos = lseek(fd, 0, SEEK_CUR);
    printf("Поточна позиція: %ld\n", pos);

    const char *new_text = "Hi!!!";
    written = pwrite(fd, new_text, strlen(new_text), 0);
    if (written < 0) {
        perror("pwrite");
        close(fd);
        return 1;
    }

    pos = lseek(fd, 0, SEEK_CUR);
    printf("Позиція після pwrite: %ld\n", pos);

    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }

    char buf[20];
    ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return 1;
    }
    buf[bytes_read] = '\0';
    
    printf("Вміст файлу: %s\n", buf);

    close(fd);
    return 0;
}
