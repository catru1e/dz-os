#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        const char msg[] = "Usage: ./simple-copy <source-file> <destination-file>\n";
        write(2, msg, sizeof(msg) - 1);
        return 1;
    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        const char msg[] = "Error: cannot open source file\n";
        write(2, msg, sizeof(msg) - 1);
        return 1;
    }

    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        const char msg[] = "Error: cannot open destination file\n";
        write(2, msg, sizeof(msg) - 1);
        close(src_fd);
        return 1;
    }

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t bytes_written = write(dst_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            const char msg[] = "Error: write failed\n";
            write(2, msg, sizeof(msg) - 1);
            close(src_fd);
            close(dst_fd);
            return 1;
        }
    }

    if (bytes_read == -1) {
        const char msg[] = "Error: read failed\n";
        write(2, msg, sizeof(msg) - 1);
        close(src_fd);
        close(dst_fd);
        return 1;
    }

    close(src_fd);
    close(dst_fd);

    const char msg[] = "File copied successfully\n";
    write(1, msg, sizeof(msg) - 1);

    return 0;
}
