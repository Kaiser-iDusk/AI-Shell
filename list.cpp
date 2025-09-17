#include "list.hpp"

CommandHistory::~CommandHistory() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

void CommandHistory::add(const std::string& command) {
    if (command.empty()) return;
    
    Node* newNode = new Node{command, tail, nullptr};
    if (tail) {
        tail->next = newNode;
    }
    tail = newNode;
    if (!head) {
        head = newNode;
    }
    size++;

    if (size > MAX_SIZE) {
        Node* temp = head;
        head = head->next;
        if (head) head->prev = nullptr;
        delete temp;
        size--;
    }
    current_pos = nullptr;
}

std::string CommandHistory::get_prev() {
    if (!current_pos) {
        current_pos = tail;
    } else if (current_pos->prev) {
        current_pos = current_pos->prev;
    }
    return current_pos ? current_pos->command : "";
}

std::string CommandHistory::get_next() {
    if (current_pos && current_pos->next) {
        current_pos = current_pos->next;
        return current_pos->command;
    }
    current_pos = nullptr;
    return "";
}