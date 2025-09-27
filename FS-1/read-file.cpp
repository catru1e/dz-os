#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        const char msg[] = "Usage: ./read-file <filename>\n";
        write(2, msg, sizeof(msg) - 1);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        const char msg[] = "Error opening file\n";
        write(2, msg, sizeof(msg) - 1);
        return 1;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes_read);
    }

    if (bytes_read == -1) {
        const char msg[] = "Error reading file\n";
        write(2, msg, sizeof(msg) - 1);
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
