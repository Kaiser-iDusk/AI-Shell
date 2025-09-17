#include "utils.hpp"
#include <unistd.h> // For getcwd
#include <limits.h> // For PATH_MAX
#include <sys/wait.h>

std::string get_current_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return std::string(cwd);
    } else {
        perror("getcwd() error");
        return "";
    }
}

std::vector<std::string> parse_input(const std::string& input) {
    std::vector<std::string> args;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        args.push_back(token);
    }
    return args;
}

void print_error(const std::string& message) {
    std::cerr << COLOR_RED << "Error: " << message << COLOR_RESET << std::endl;
}

void print_msg(const std::string& message){
    std::cout << COLOR_GREEN << message << COLOR_RESET << " ";
}

std::string get_ls_output() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return "Error creating pipe.";
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return "Error forking process.";
    }

    if (pid == 0) { // Child process
        close(pipefd[0]); // Close the read end in the child
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe's write end
        close(pipefd[1]); // Close the original write end

        // Execute 'ls'. Its output now goes into the pipe.
        execlp("ls", "ls", (char*)NULL);

        // execlp only returns on error
        perror("execlp ls");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(pipefd[1]); // Close the write end in the parent

        std::string output;
        char buffer[512];
        ssize_t count;

        // Read the output from the pipe's read end
        while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, count);
        }

        close(pipefd[0]);
        waitpid(pid, NULL, 0); // Wait for the child to finish

        // Replace newlines with spaces for a cleaner prompt
        for (char& c : output) {
            if (c == '\n') {
                c = ' ';
            }
        }
        return output;
    }
}