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

    // Step 2: Send LOGIN message
    std::ostringstream login_message;
    login_message << "LOGIN " << username << "\n";
    send_message(rio, fd, login_message.str());

    std::string response = receive_message(rio);
    if (response.substr(0, 2) != "OK") {
        std::cerr << "Error: LOGIN failed - " << response;
        Close(fd);
        return 1;
    }

    // Step 3: Send GET message
    std::ostringstream get_message;
    get_message << "GET " << table << " " << key << "\n";
    send_message(rio, fd, get_message.str());

    response = receive_message(rio);
    if (response.substr(0, 2) != "OK") {
        std::cerr << "Error: GET failed - " << response;
        Close(fd);
        return 1;
    }

// Step 4: Send TOP message to retrieve the value
send_message(rio, fd, "TOP\n");

// Expect a "DATA value" response from the server
response = receive_message(rio);
if (response.substr(0, 4) != "DATA") {
    std::cerr << "Error: Unexpected TOP response - " << response;
    Close(fd);
    return 1;
}

// Extract and print the value from the response
std::string value = response.substr(5); // Skip the "DATA " prefix
std::cout << value;

// Step 5: Send BYE message to gracefully close the connection
send_message(rio, fd, "BYE\n");


// Step 7: Close the connection
Close(fd);
return 0;
}
