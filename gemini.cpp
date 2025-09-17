#include "gemini.hpp"
#include "json.hpp"
#include <curl/curl.h>
#include <fstream> 

using json = nlohmann::json;

// --- NEW FUNCTION ---
// Reads a specific key from a .env file in the current directory.
std::string loadApiKeyFromEnv() {
    std::ifstream env_file(".env");
    if (!env_file.is_open()) {
        return ""; // Return empty if .env file doesn't exist
    }

    std::string line;
    std::string key = "GEMINI_API_KEY";
    while (std::getline(env_file, line)) {
        // Find the position of the '='
        size_t equals_pos = line.find('=');
        if (equals_pos != std::string::npos) {
            // Extract the key part and the value part
            std::string current_key = line.substr(0, equals_pos);
            if (current_key == key) {
                return line.substr(equals_pos + 1);
            }
        }
    }
    return ""; // Return empty if key was not found
}


// This callback is only used by this file, so it can stay here.
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}


std::string get_command_from_gemini(const std::string& query, const std::string& context) {
    // MODIFICATION: Load the API key from the .env file
    std::string api_key = loadApiKeyFromEnv();
    if (api_key.empty()) {
        print_error("GEMINI_API_KEY not found in .env file.");
        return "";
    }

    // The rest of this function remains the same
    std::string readBuffer;
    CURL* curl = curl_easy_init();
    if (!curl) { print_error("Failed to initialize libcurl."); return ""; }
    
    json request_body;
    request_body["contents"][0]["parts"][0]["text"] = "You are an expert command-line assistant in Unix based systems. Give requested command for the query by processing the current diretory and its contents. Don't give anything else and give it in simple text and not in markdown. Since most of the commands will be in bash, don't prepend `bash` because it will fail. Follow the instructions carefully and nothing else... " + context + ". User Request: \"" + query + "\". Command:";
    std::string request_body_str = request_body.dump();
    
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent";
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
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) { 
        print_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res))); 
        return ""; 
    }
    
    try {
        json response_json = json::parse(readBuffer);
        std::string command = response_json["candidates"][0]["content"]["parts"][0]["text"];
        size_t first = command.find_first_not_of(" \n\r\t`");
        size_t last = command.find_last_not_of(" \n\r\t`");
        if (first == std::string::npos || last == std::string::npos) return "";
        return command.substr(first, (last - first + 1));
    } catch (...) {
        print_error("An unknown error occurred while parsing API response.");
        return "";
    }
}