#pragma once
#include "utils.hpp"

class CommandHistory {
private:
    struct Node {
        std::string command;
        Node *prev;
        Node *next;
    };
    Node *head = nullptr;
    Node *tail = nullptr;
    Node *current_pos = nullptr;
    int size = 0;
    const int MAX_SIZE = 50;

public:
    ~CommandHistory();
    void add(const std::string& command);
    std::string get_prev();
    std::string get_next();
};