#include <iostream>
#include <string>
#include <sstream>
#include "csapp.h"

// Helper function to send a message using rio_writen
void send_message(rio_t &rio, int fd, const std::string &message) {
  Rio_writen(fd, message.c_str(), message.length());
}

// Helper function to receive a message using rio_readlineb
std::string receive_message(rio_t &rio) {
    char buffer[MAXLINE];
    ssize_t n = Rio_readlineb(&rio, buffer, MAXLINE);
    if (n == 0) {
        std::cerr << "Error: Server closed the connection unexpectedly\n";
        exit(1);
    }
    buffer[n] = '\0';  // Null-terminate the response
    return std::string(buffer);
}

// Helper function to extract quoted text from a response
std::string extract_quoted_text(const std::string &response) {
    size_t start = response.find('\"');
    size_t end = response.rfind('\"');
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return response.substr(start + 1, end - start - 1); // Extract text between quotes
    }
    return "Unknown error"; // Fallback if quotes are missing or malformed
}

int main(int argc, char **argv) {
  if (argc != 7) {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];
  std::string value = argv[6];

  // Step 1: Open TCP connection to the server
  int fd = Open_clientfd(hostname.c_str(), port.c_str());
  if (fd < 0) {
    std::cerr << "Error: Couldn't connect to server\n";
    return 1;
  }

  rio_t rio;
  Rio_readinitb(&rio, fd);  // Initialize robust I/O

  // Step 2: Send LOGIN message
  std::ostringstream login_message;
  login_message << "LOGIN " << username << "\n";
  send_message(rio, fd, login_message.str());

  std::string response = receive_message(rio);
  if (response.substr(0, 2) != "OK") {
    std::string error_message = extract_quoted_text(response);
    std::cerr << "Error: " << error_message << std::endl;
    Close(fd);
    return 1;
  }

  // Step 3: Send PUSH message
  std::ostringstream push_message;
  push_message << " PUSH " << value << "\n";
  send_message(rio, fd, push_message.str());

  response = receive_message(rio);
  if (response.substr(0, 2) != "OK") {
    std::string error_message = extract_quoted_text(response);
    std::cerr << "Error: " << error_message << std::endl;
    Close(fd);
    return 1;
  }

  // Step 4: Send SET message
  std::ostringstream set_message;
  set_message << "SET " << table << " " << key << "\n";
  send_message(rio, fd, set_message.str());

  response = receive_message(rio);
  if (response.substr(0, 2) != "OK") {
    std::string error_message = extract_quoted_text(response);
    std::cerr << "Error: " << error_message << std::endl;
    Close(fd);
    return 1;
  }

  // Step 5: Send BYE message to gracefully close the connection
  send_message(rio, fd, "BYE\n");

  // Step 6: Close the connection
  Close(fd);
  return 0;
}
