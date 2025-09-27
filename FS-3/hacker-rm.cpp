#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>


static int write_all(int fd, const void *buf, size_t count) {
    const char *p = (const char *)buf;
    size_t left = count;
    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            return -1;
        }
        p += w;
        left -= (size_t)w;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        const char msg[] = "Usage: ./hacker-rm <file-to-erase>\n";
        write(2, msg, sizeof(msg) - 1);
        return 1;
    }

    const char *path = argv[1];

    struct stat st;
    if (stat(path, &st) == -1) {
        write(2, "Error: stat failed\n", strlen("Error: stat failed\n"));
        return 1;
    }

    if (!S_ISREG(st.st_mode)) {
        write(2, "Error: target is not a regular file\n", strlen("Error: target is not a regular file\n"));
        return 1;
    }

    off_t filesize = st.st_size;

    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        write(2, "Error: cannot open file\n", strlen("Error: cannot open file\n"));
        return 1;
    }

    if (filesize > 0) {
        const size_t CHUNK = 8192;
        char zeros[CHUNK];
        /* initialize buffer of zeros */
        for (size_t i = 0; i < CHUNK; ++i) zeros[i] = '\0';

        off_t written_total = 0;
        /* Seek to beginning */
        if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
            write(2, "Error: lseek failed\n", strlen("Error: lseek failed\n"));
            close(fd);
            return 1;
        }

        while (written_total < filesize) {
            size_t to_write = (size_t)((filesize - written_total) < (off_t)CHUNK ? (filesize - written_total) : CHUNK);
            if (write_all(fd, zeros, to_write) == -1) {
                write(2, "Error: write failed\n", strlen("Error: write failed\n"));
                close(fd);
                return 1;
            }
            written_total += (off_t)to_write;
        }
    }

    if (close(fd) == -1) {
        write(2, "Error: close failed\n", strlen("Error: close failed\n"));
    }

    if (unlink(path) == -1) {
        write(2, "Error: unlink failed\n", strlen("Error: unlink failed\n"));
        return 1;
    }

    const char okmsg[] = "File securely wiped (bytes set to '\\0') and removed.\n";
    write(1, okmsg, sizeof(okmsg) - 1);

    return 0;
}
