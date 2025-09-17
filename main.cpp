#include "utils.hpp"
#include "list.hpp"
#include "handler.hpp"
#include "gemini.hpp"
#include <iostream>
#include <unistd.h> // For chdir

int main() {
    CommandHistory history;
    std::string input;
    
    while (true) {
        reap_background_processes();
        
        std::cout << COLOR_BLUE << get_current_directory() << COLOR_RESET << " $ " << std::flush;

        // MODIFICATION: Reverted to standard std::getline
        if (!std::getline(std::cin, input)) {
            std::cout << "exit" << std::endl; // Handle Ctrl+D
            break;
        }

        // MODIFICATION: Check for special history-navigation sequences
        // The literal escape code for UP arrow is '\033[A'
        // The literal escape code for DOWN arrow is '\033[B'
        bool from_history = false;
        if (input == "\033[A") {
            input = history.get_prev();
            std::cout << COLOR_GREEN << "Recall: " << COLOR_RESET << input << std::endl;
            from_history = true;
        } else if (input == "\033[B") {
            input = history.get_next();
            std::cout << COLOR_GREEN << "Recall: " << COLOR_RESET << input << std::endl;
            from_history = true;
        }

        // Only add commands to history if they are new, not recalled ones.
        if (!from_history) {
            history.add(input);
        }
        
        // query block
        if (input.rfind("]", 0) == 0) {
            std::string query = input.substr(1);
            std::cout << "Asking Gemini... " << std::endl;

            // 1. Get all context
            std::string current_dir = get_current_directory();
            std::string ls_output = get_ls_output();
            std::string full_context = "Current directory: " + current_dir + ". Files in directory: " + ls_output;

            // 2. Pass the full context to Gemini
            std::string ai_command = get_command_from_gemini(query, full_context);

            if (ai_command.empty()) {
                print_error("AI failed to provide a command.");
                continue;
            }
            print_msg("AI Suggests: `" + ai_command + "`. Run it? [y/n] ");
            std::string confirmation;
            std::getline(std::cin, confirmation);
            if (confirmation != "y" && confirmation != "Y") {
                std::cout << "Command cancelled." << std::endl;
                continue;
            }
            input = ai_command;
        }
        
        std::vector<std::string> args = parse_input(input);
        if (args.empty()) {
            continue;
        }
        
        bool is_background = false;
        if (!args.empty() && args.back() == "&") {
            is_background = true;
            args.pop_back();
            if (args.empty()) continue;
        }

        if (args[0] == "exit") {
            break;
        } else if (args[0] == "cd") {
            if (args.size() < 2) {
                print_error("cd: missing argument.");
            } else {
                if (chdir(args[1].c_str()) != 0) {
                    perror((COLOR_RED + "cd").c_str());
                }
            }
        } else {
            execute_external_command(args, is_background);
        }
    }
    return 0;
}