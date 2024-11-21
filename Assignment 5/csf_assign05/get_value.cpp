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


int handle_error(int fd, const std::string &response) {
  if (response.substr(0, 2) != "OK") {
    std::string error_message = extract_quoted_text(response);
    std::cerr << "Error: " << error_message << std::endl;
    Close(fd);
    return 1;
  }

  return 0;
}

// Send LOGIN message
int LOGIN(rio_t &rio, int fd, std::string username) {
  std::ostringstream login_message;
  login_message << "LOGIN " << username << "\n";
  send_message(rio, fd, login_message.str());
  std::string response = receive_message(rio);
  return handle_error(fd, response);
}

// Send GET message
int GET(rio_t &rio, int fd, std::string table, std::string key) {
  std::ostringstream get_message;
  get_message << "GET " << table << " " << key << "\n";
  send_message(rio, fd, get_message.str());

  std::string response = receive_message(rio);
  return handle_error(fd, response);
}

int TOP(rio_t &rio, int fd) {
  send_message(rio, fd, "TOP\n");

  // Expect a "DATA value" response from the server
  std::string response = receive_message(rio);
  if (response.substr(0, 4) != "DATA") {
    std::string error_message = extract_quoted_text(response);
    std::cerr << "Error: " << error_message << std::endl;
    Close(fd);
    return 1;
  }

  // Extract and print the value from the response
  std::string value = response.substr(5); // Skip the "DATA " prefix
  std::cout << value;

  return 0;
}

int execute_get_value(rio_t &rio, int fd, std::string username, std::string table, std::string key) {
  // Step 2: Send LOGIN message
  if (LOGIN(rio, fd, username) != 0) {
    return 1;
  } 

  // Step 3: Send GET message
  if (GET(rio, fd, table, key) != 0) {
    return 1;
  } 

  // Step 4: Send TOP message to retrieve the data
  if (TOP(rio, fd) != 0) {
    return 1;
  }

  // Step 5: Send BYE message to gracefully close the connection
  send_message(rio, fd, "BYE\n");
  return 0;
}


int main(int argc, char **argv) {
  if (argc != 6) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

  // Step 1: Open TCP connection to the server
  int fd = Open_clientfd(hostname.c_str(), port.c_str());
  if (fd < 0) {
    std::cerr << "Error: Couldn't connect to server\n";
    return 1;
  }

  rio_t rio;
  Rio_readinitb(&rio, fd);  // Initialize robust I/O

  // Step 2-5: Execute get_value
  if (execute_get_value(rio, fd, username, table, key) != 0) {
    return 1;
  }

  // Step 6: Close the connection
  Close(fd);
  return 0;
}
