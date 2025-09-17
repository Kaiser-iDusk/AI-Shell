#pragma once
#include "utils.hpp"

void reap_background_processes();
void execute_external_command(const std::vector<std::string>& args, bool is_background);