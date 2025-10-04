#include <iostream>
#include <string>
#include <fcntl.h>      // open
#include <unistd.h>     // dup2, close
#include <cstdlib>      // exit, EXIT_FAILURE

void initialize(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    // перенаправить stdin на этот файл =)
    if (dup2(fd, STDIN_FILENO) == -1) {
        perror("Error redirecting stdin");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

int main(int argc, char** argv)
{
    initialize(argc, argv);

    std::string input;
    if (!(std::cin >> input)) {
        std::cerr << "Error reading input" << std::endl;
        return EXIT_FAILURE;
    }

    std::reverse(input.begin(), input.end());

    std::cout << input << std::endl;
    return 0;
}
