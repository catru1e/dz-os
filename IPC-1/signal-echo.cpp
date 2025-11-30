#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <cstring>

void handler(int sig, siginfo_t* info, void*) {
    pid_t sender_pid = info->si_pid;
    uid_t sender_uid = info->si_uid;

    struct passwd* pw = getpwuid(sender_uid);
    const char* username = pw ? pw->pw_name : "unknown";

    std::cout << "Received SIGUSR1 from PID " << sender_pid
              << ", UID " << sender_uid
              << " (" << username << ")" << std::endl;
}

int main() {
    std::cout << "Process PID = " << getpid() << std::endl;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &sa, nullptr);

    while (true) {
        sleep(10);
    }

    return 0;
}
