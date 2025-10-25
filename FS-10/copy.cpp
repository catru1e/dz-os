#include <iostream>
#include <fcntl.h>   // open
#include <unistd.h>  // read, write, lseek, close
#include <cstdlib>   // exit, EXIT_FAILURE
#include <cstring>   // strerror
#include <sys/stat.h> // fstat

#define BUF_SIZE 4096

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source> <destination>" << std::endl;
        return EXIT_FAILURE;
    }

    const char* src_path = argv[1];
    const char* dst_path = argv[2];

    int src = open(src_path, O_RDONLY);
    if (src == -1) {
        perror("open source");
        return EXIT_FAILURE;
    }
    int dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst == -1) {
        perror("open dest");
        close(src);
        return EXIT_FAILURE;
    }

    struct stat st;
    if (fstat(src, &st) == -1) {
        perror("fstat");
        close(src);
        close(dst);
        return EXIT_FAILURE;
    }

    off_t filesize = st.st_size;
    off_t pos = 0;
    ssize_t total_data = 0;
    ssize_t total_hole = 0;

    while (pos < filesize) {
        off_t data_off = lseek(src, pos, SEEK_DATA);
        if (data_off == -1) {
            data_off = filesize;
        }

        if (data_off > pos) {
            off_t hole_size = data_off - pos;
            if (lseek(dst, hole_size, SEEK_CUR) == -1) {
                perror("lseek hole");
                close(src);
                close(dst);
                return EXIT_FAILURE;
            }
            total_hole += hole_size;
            pos = data_off;
        }

        off_t hole_off = lseek(src, pos, SEEK_HOLE);
        if (hole_off == -1) {
            hole_off = filesize;
        }

        off_t data_size = hole_off - pos;

        char buf[BUF_SIZE];
        off_t copied = 0;
        while (copied < data_size) {
            size_t to_read = std::min((off_t)BUF_SIZE, data_size - copied);
            ssize_t r = pread(src, buf, to_read, pos + copied);
            if (r <= 0) {
                perror("read");
                close(src);
                close(dst);
                return EXIT_FAILURE;
            }
            ssize_t w = write(dst, buf, r);
            if (w != r) {
                perror("write");
                close(src);
                close(dst);
                return EXIT_FAILURE;
            }
            copied += r;
        }

        total_data += data_size;
        pos = hole_off;
    }

    // Если дыра в конце добавляем пробелы
    if (ftruncate(dst, filesize) == -1) {
        perror("ftruncate");
        close(src);
        close(dst);
        return EXIT_FAILURE;
    }

    close(src);
    close(dst);

    std::cout << "Successfully copied " << filesize
              << " bytes (data: " << total_data
              << ", hole: " << total_hole << ")." << std::endl;

    return 0;
}
