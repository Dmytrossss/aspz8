#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd = open("testfile.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    const char *initial_data = "Hello World!";
    ssize_t written = write(fd, initial_data, strlen(initial_data));
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    }
    
    off_t current_pos = lseek(fd, 0, SEEK_CUR);
    printf("Поточна позиція після запису: %ld\n", current_pos);

    const char *new_data = "LINUX";
    written = pwrite(fd, new_data, strlen(new_data), 0);
    if (written < 0) {
        perror("pwrite");
        close(fd);
        return 1;
    }

    current_pos = lseek(fd, 0, SEEK_CUR);
    printf("Позиція після pwrite: %ld\n", current_pos);

    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }

    char buffer[20];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return 1;
    }
    buffer[bytes_read] = '\0';
    printf("Новий вміст файлу: %s\n", buffer);

    close(fd);
    return 0;
}
