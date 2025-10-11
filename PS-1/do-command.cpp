#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>
#include <vector>
#include <string>

void do_command(char** argv) {
    using namespace std::chrono;

    // Start timer
    auto start = high_resolution_clock::now();

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Error: failed to fork process\n";
        return;
    }

    if (pid == 0) {
        // Child process: execute command
        execvp(argv[0], argv);
        // If execvp returns, there was an error
        std::cerr << "Error: failed to execute command '" << argv[0] << "': " << strerror(errno) << "\n";
        _exit(EXIT_FAILURE);
    }

    // Parent process: wait for child
    int status;
    waitpid(pid, &status, 0);

    // End timer
    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    // Determine exit code
    int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    std::cout << "\nCommand completed with " << exit_code
              << " exit code and took " << elapsed.count() << " seconds.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [args...]\n";
        return 1;
    }

    std::vector<char*> exec_args;
    for (int i = 1; i < argc; ++i) {
        exec_args.push_back(argv[i]);
    }
    exec_args.push_back(nullptr);

    do_command(exec_args.data());

    return 0;
}
