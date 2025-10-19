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

// Execution of one command (without ;, &&, ||)
int execute_command(std::vector<std::string> toks, bool silent) {
    if (toks.empty()) return 0;

    int redirect_fd = -1;
    bool append = false;
    for (size_t i = 0; i < toks.size(); ++i) {
        if (toks[i] == ">" || toks[i] == ">>") {
            if (i + 1 >= toks.size()) {
                std::cerr << "syntax error near redirection\n";
                return -1;
            }
            append = (toks[i] == ">>");
            std::string filename = toks[i + 1];
            toks.erase(toks.begin() + i, toks.begin() + i + 2);
            int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
            redirect_fd = open(filename.c_str(), flags, 0644);
            if (redirect_fd < 0) {
                std::perror("open");
                return -1;
            }
            break;
        }
    }

    std::vector<char*> argv;
    for (auto &s : toks)
        argv.push_back(const_cast<char*>(s.c_str()));
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) {
        std::perror("fork");
        return -1;
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
                std::perror("open log");
                _exit(EXIT_FAILURE);
            }
            if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0) {
                std::perror("dup2");
                close(fd);
                _exit(EXIT_FAILURE);
            }
            close(fd);
        }

        if (redirect_fd != -1) {
            if (dup2(redirect_fd, STDOUT_FILENO) < 0 || dup2(redirect_fd, STDERR_FILENO) < 0) {
                std::perror("dup2");
                close(redirect_fd);
                _exit(EXIT_FAILURE);
            }
            close(redirect_fd);
        }

        // execvp: will use PATH (modified) to search for named commands
        execvp(argv[0], argv.data());
        // If execvp returns, it's an error
        std::cerr << "exec failed for '" << argv[0] << "': " << strerror(errno) << "\n";
        _exit(EXIT_FAILURE);
    } else {
        // PARENT: wait for child to finish
        int status = 0;
        waitpid(pid, &status, 0);
        if (redirect_fd != -1)
            close(redirect_fd);

        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        else
            return -1;
    }
}

void process_line(const std::string &line) {
    std::istringstream iss(line);
    std::string segment;
    std::vector<std::string> parts;
    std::vector<std::string> ops;

    std::string token;
    while (iss >> token) {
        if (token == ";" || token == "&&" || token == "||") {
            ops.push_back(token);
            parts.push_back(segment);
            segment.clear();
        } else {
            if (!segment.empty()) segment += " ";
            segment += token;
        }
    }
    if (!segment.empty()) parts.push_back(segment);

    int last_status = 0;
    for (size_t i = 0; i < parts.size(); ++i) {
        std::string trimmed = parts[i];
        size_t start = trimmed.find_first_not_of(" \t\r\n");
        size_t end = trimmed.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) continue; // empty line
        trimmed = trimmed.substr(start, end - start + 1);

        bool silent = false;
        std::vector<std::string> toks = split_tokens(trimmed);
        if (toks.empty()) continue;

        if (toks[0] == "silent") {
            silent = true;
            toks.erase(toks.begin());
            if (toks.empty()) {
                std::cerr << "silent: missing command\n";
                continue;
            }
        }

        if (i > 0) {
            if (ops[i - 1] == "&&" && last_status != 0) continue;
            if (ops[i - 1] == "||" && last_status == 0) continue;
        }

        last_status = execute_command(toks, silent);
    }
}

int main() {
    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) {
            std::cout << '\n';
            return 0;
        }

        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        std::string trimmed = line.substr(start, end - start + 1);

        if (trimmed == "exit")
            break;

        process_line(trimmed);
    }

    return 0;
}
