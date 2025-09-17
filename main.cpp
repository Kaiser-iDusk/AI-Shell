#include <iostream>
#include <string>
#include <vector>
#include <sstream>      // For std::stringstream
#include <unistd.h>     // For fork, execvp, chdir, getcwd
#include <sys/wait.h>   // For waitpid
#include <limits.h>     // For PATH_MAX

// External libraries (make sure they are installed)
#include <curl/curl.h>  // For making HTTP requests
#include "json.hpp"     // For parsing JSON (nlohmann/json)

// Use the nlohmann::json namespace for convenience
using json = nlohmann::json;

// ANSI color codes for UI enhancements
const std::string COLOR_BLUE = "\033[1;34m";
const std::string COLOR_RED = "\033[1;31m";
const std::string COLOR_GREEN = "\033[1;32m";
const std::string COLOR_RESET = "\033[0m";

// --- Function Declarations ---

// Core shell functions
void execute_external_command(const std::vector<std::string>& args);
std::vector<std::string> parse_input(const std::string& input);

// AI integration functions
std::string get_command_from_gemini(const std::string& query, const std::string& context);
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

// Helper functions
void print_error(const std::string& message);
void print_msg(const std::string& message);
std::string get_current_directory();

// --- Main Shell Logic ---

int main() {
    std::string input;
    std::string current_dir;

    while (true) {
        current_dir = get_current_directory();
        std::cout << COLOR_BLUE << current_dir << COLOR_RESET << " $ ";
        
        std::getline(std::cin, input);
        if (std::cin.eof()) { // Handle Ctrl+D
            std::cout << "exit" << std::endl;
            break;
        }

        if (input.empty()) {
            continue;
        }

        // Check if it's an AI query
        if (input.rfind("]", 0) == 0) {
            std::string query = input.substr(1); // Remove the ']'
            std::cout << "Asking AI... (query: \"" << query << "\")" << std::endl;
            
            // Get command from Gemini and replace the input
            input = get_command_from_gemini(query, "Current directory is " + current_dir);

            if (input.empty()) {
                print_error("AI failed to provide a command.");
                continue;
            }

            print_msg("AI Suggests: `" + input + "`. Run it? [y/n] ");
            std::string confirmation;
            std::getline(std::cin, confirmation);
            if (confirmation != "y" && confirmation != "Y") {
                std::cout << "Command cancelled." << std::endl;
                continue;
            }
        }
        
        std::vector<std::string> args = parse_input(input);
        if (args.empty()) {
            continue;
        }

        // --- Handle Built-in Commands ---
        if (args[0] == "exit") {
            break;
        } else if (args[0] == "cd") {
            if (args.size() < 2) {
                print_error("cd: missing argument.");
            } else {
                if (chdir(args[1].c_str()) != 0) {
                    perror((COLOR_RED + "cd").c_str()); // perror prints system error
                }
            }
        } else {
            // --- Handle External Commands ---
            execute_external_command(args);
        }
    }

    return 0;
}


// --- Function Implementations ---

/**
 * @brief Splits a string into a vector of arguments.
 */
std::vector<std::string> parse_input(const std::string& input) {
    std::vector<std::string> args;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        args.push_back(token);
    }
    return args;
}

/**
 * @brief Executes an external command using fork() and execvp().
 */
void execute_external_command(const std::vector<std::string>& args) {
    // Convert vector<string> to vector<char*> for execvp
    std::vector<char*> c_args;
    for (const auto& arg : args) {
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr); // execvp requires a null-terminated array

    pid_t pid = fork();

    if (pid == -1) {
        print_error("Failed to fork.");
        return;
    } else if (pid == 0) {
        // --- Child Process ---
        if (execvp(c_args[0], c_args.data()) == -1) {
            print_error("Command not found: " + args[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        // --- Parent Process ---
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * @brief Prints a message to stderr in red.
 */
void print_error(const std::string& message) {
    std::cerr << COLOR_RED << "Error: " << message << COLOR_RESET << std::endl;
}

/**
 * @brief Prints successful AI msg to stdout in green.
 */
void print_msg(const std::string& message){
    std::cout << COLOR_GREEN << message << COLOR_RESET << " ";
}

/**
 * @brief Gets the current working directory.
 */
std::string get_current_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return std::string(cwd);
    } else {
        perror("getcwd() error");
        return std::string("");
    }
}

// --- Gemini API Integration ---

/**
 * @brief Callback function for libcurl to write received data into a string.
 */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Calls the Gemini API to get a shell command from a natural language query.
 *
 * NOTE: This is a detailed placeholder. You MUST replace "YOUR_API_KEY"
 * with your actual Google Gemini API key.
 */
std::string get_command_from_gemini(const std::string& query, const std::string& context) {
    std::string api_key = "AIzaSyC-NSZlSGpzXYKf0reqWcsExizhPfAnw48"; // <-- IMPORTANT: REPLACE THIS!
    if (api_key == "YOUR_API_KEY") {
        print_error("Gemini API key is not set. Please edit the source code.");
        return ""; // Return empty on failure
    }

    std::string readBuffer;
    CURL* curl = curl_easy_init();
    if (!curl) {
        print_error("Failed to initialize libcurl.");
        return "";
    }

    // Prepare the JSON payload for the API
    json request_body;
    request_body["contents"][0]["parts"][0]["text"] = 
        "You are an expert command-line assistant for a Linux system. "
        "Based on the user's request and context, provide a single, executable shell command and nothing else. "
        "Context: " + context + ". "
        "User Request: \"" + query + "\". "
        "Command:";
    
    std::string request_body_str = request_body.dump();

    // Set up the curl request
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?";
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    std::string api_key_header = "X-goog-api-key: " + api_key;
    headers = curl_slist_append(headers, api_key_header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body_str.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        print_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
        curl_easy_cleanup(curl);
        return "";
    }

    curl_easy_cleanup(curl);
    
    // Parse the JSON response
    try {
        json response_json = json::parse(readBuffer);
        std::string command = response_json["candidates"][0]["content"]["parts"][0]["text"];
        // Clean up the command (AI might add backticks or newlines)
        size_t first = command.find_first_not_of(" \n\r\t`");
        size_t last = command.find_last_not_of(" \n\r\t`");
        if (first == std::string::npos || last == std::string::npos) return "";
        return command.substr(first, (last - first + 1));

    } catch (json::parse_error& e) {
        print_error("JSON parse error: " + std::string(e.what()));
        print_error("Received from API: " + readBuffer);
        return "";
    } catch (...) {
        print_error("An unknown error occurred while parsing the API response.");
        return "";
    }
}
