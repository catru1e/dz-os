// main.cpp
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

static std::vector<std::string> split_tokens(const std::string &line) {
    std::vector<std::string> toks;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok)
        toks.push_back(tok);
    return toks;
}

int main() {
    std::string line;

    while (true) {
        // Prompt
        std::cout << "> " << std::flush;

        if (!std::getline(std::cin, line)) {
            // EOF (Ctrl-D) -> exit
            std::cout << '\n';
            return 1;
        }

        // trim leading/trailing whitespace (simple)
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue; // empty line
        size_t end = line.find_last_not_of(" \t\r\n");
        std::string trimmed = line.substr(start, end - start + 1);

        if (trimmed == "exit") {
            return EXIT_SUCCESS;
        }

        bool silent = false;
        std::vector<std::string> toks = split_tokens(trimmed);
        if (toks.empty()) continue;

        if (toks[0] == "silent") {
            silent = true;
            // remove "silent" token
            toks.erase(toks.begin());
            if (toks.empty()) {
                std::cerr << "silent: missing command\n";
                continue;
            }
        }

        // Prepare argv for execvp
        std::vector<char*> argv;
        argv.reserve(toks.size() + 1);
        for (auto &s : toks) argv.push_back(const_cast<char*>(s.c_str()));
        argv.push_back(nullptr);

        pid_t pid = fork();
        if (pid < 0) {
            std::perror("fork");
            continue;
        }

        if (pid == 0) {
            // CHILD
            // 1) Prepend current directory to PATH in child's environment
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) == nullptr) {
                // fallback: if can't get cwd, keep existing PATH
                // but we continue
            } else {
                const char *oldpath = getenv("PATH");
                std::string newpath;
                if (oldpath && std::strlen(oldpath) > 0) {
                    newpath = std::string(cwd) + ":" + oldpath;
                } else {
                    newpath = std::string(cwd);
                }
                // set environment variable only for child
                if (setenv("PATH", newpath.c_str(), 1) != 0) {
                    std::perror("setenv");
                    // don't exit; still try exec
                }
            }

            // 2) If silent -> redirect stdout and stderr to PID.log
            if (silent) {
                pid_t mypid = getpid();
                std::string fname = std::to_string(mypid) + ".log";
                int fd = open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    std::cerr << "failed to open " << fname << " : " << strerror(errno) << "\n";
                    _exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0) {
                    std::perror("dup2");
                    close(fd);
                    _exit(EXIT_FAILURE);
                }
                close(fd);
            }

            // execvp: will use PATH (modified) to search for named commands
            execvp(argv[0], argv.data());
            // If execvp returns, it's an error
            std::cerr << "exec failed for '" << argv[0] << "': " << strerror(errno) << "\n";
            _exit(EXIT_FAILURE);
        } else {
            // PARENT: wait for child to finish
            int status = 0;
            pid_t w = waitpid(pid, &status, 0);
            if (w == -1) {
                std::perror("waitpid");
            }
        }
    }

    return EXIT_SUCCESS;
}
