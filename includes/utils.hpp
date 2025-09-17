#pragma once // Prevents the header from being included multiple times

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// ANSI color codes for UI enhancements
const std::string COLOR_BLUE = "\033[1;34m";
const std::string COLOR_RED = "\033[1;31m";
const std::string COLOR_GREEN = "\033[1;32m";
const std::string COLOR_RESET = "\033[0m";

// Function Declarations
std::string get_current_directory();
std::string get_ls_output();
std::vector<std::string> parse_input(const std::string& input);
void print_error(const std::string& message);
void print_msg(const std::string& message);