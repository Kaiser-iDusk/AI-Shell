#include "handler.hpp"
#include <unistd.h>
#include <sys/wait.h>

void reap_background_processes() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        std::cout << std::endl << COLOR_GREEN << "Background process " << pid << " finished." << COLOR_RESET << std::endl;
    }
}

void execute_external_command(const std::vector<std::string>& args, bool is_background) {
    std::vector<char*> c_args;
    for (const auto& arg : args) {
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    pid_t pid = fork();

    if (pid == -1) {
        print_error("Failed to fork.");
        return;
    } else if (pid == 0) { // Child
        if (execvp(c_args[0], c_args.data()) == -1) {
            print_error("Command not found: " + args[0]);
            exit(EXIT_FAILURE);
        }
    } else { // Parent
        if (is_background) {
            std::cout << "Started background process with PID: " << pid << std::endl;
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}