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

// Send BEGIN message if using a transaction
int BEGIN(rio_t &rio, int fd, bool use_transaction) {
  if (use_transaction) {
    std::ostringstream begin_message;
    begin_message << "BEGIN\n";
    send_message(rio, fd, begin_message.str());

    std::string response = receive_message(rio);
    return handle_error(fd, response);
  }
  return 0;
} 

// Send GET message
int GET(rio_t &rio, int fd, std::string table, std::string key) {
  std::ostringstream get_message;
  get_message << "GET " << table << " " << key << "\n";
  send_message(rio, fd, get_message.str());

  std::string response = receive_message(rio);
  return handle_error(fd, response);
}

// Send PUSH message
int PUSH(rio_t &rio, int fd) {
  std::ostringstream push_message;
  push_message << " PUSH 1 " << "\n";
  send_message(rio, fd, push_message.str());

  std::string response = receive_message(rio);
  return handle_error(fd, response);
}

// Send ADD message
int ADD(rio_t &rio, int fd) {
  std::ostringstream add_message;
  add_message << " ADD\n";
  send_message(rio, fd, add_message.str());

  std::string response = receive_message(rio);
  return handle_error(fd, response);
}

// Send SET message
int SET(rio_t &rio, int fd, std::string table, std::string key) {
  std::ostringstream set_message;
  set_message << "SET " << table << " " << key << "\n";
  send_message(rio, fd, set_message.str());

  std::string response = receive_message(rio);
  return handle_error(fd, response);
}

// Send COMMIT message if using a transaction
int COMMIT(rio_t &rio, int fd, bool use_transaction) {
  if (use_transaction) {
    std::ostringstream commit_message;
    commit_message << "COMMIT\n";
    send_message(rio, fd, commit_message.str());

    std::string response = receive_message(rio);
    return handle_error(fd, response);
  }
  return 0;
}

int execute_incr_value(rio_t &rio, int fd, bool use_transaction, std::string username, std::string table, std::string key) {
  // Step 2: Send LOGIN message
  if (LOGIN(rio, fd, username) != 0) {
    return 1;
  } 

  // Step 3: Send BEGIN message if using a transaction
  if (BEGIN(rio, fd, use_transaction) != 0) {
    return 1;
  }

  // Step 4: Send GET message
  if (GET(rio, fd, table, key) != 0) {
    return 1;
  } 

  // Step 5: Send PUSH message
  if (PUSH(rio, fd) != 0) {
    return 1;
  }

  // Step 6: Send ADD message
  if (ADD(rio, fd) != 0) {
    return 1;
  }

  // Step 7: Send SET message
  if (SET(rio, fd, table, key) != 0) {
    return 1;
  }

  // Step 8: Send COMMIT message if using a transaction
  if (COMMIT(rio, fd, use_transaction) != 0) {
    return 1;
  }

  // Step 9: Send BYE message to gracefully close the connection
  send_message(rio, fd, "BYE\n");
  return 0;
}

int main(int argc, char **argv) {
  if ( argc != 6 && (argc != 7 || std::string(argv[1]) != "-t") ) {
    std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -t      execute the increment as a transaction\n";
    return 1;
  }

  int count = 1;

  bool use_transaction = false;
  if ( argc == 7 ) {
    use_transaction = true;
    count = 2;
  }

  std::string hostname = argv[count++];
  std::string port = argv[count++];
  std::string username = argv[count++];
  std::string table = argv[count++];
  std::string key = argv[count++];

  // Step 1: Open TCP connection to the server
  int fd = Open_clientfd(hostname.c_str(), port.c_str());
  if (fd < 0) {
    std::cerr << "Error: Couldn't connect to server\n";
    return 1;
  }

  rio_t rio;
  Rio_readinitb(&rio, fd);  // Initialize robust I/O

  // Step 2 - 9: Execute Incr Value
  if (execute_incr_value(rio, fd, use_transaction, username, table, key) != 0) {
    return 1;
  }  

  // Step 10: Close the connection
  Close(fd);
  return 0;
}
