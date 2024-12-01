#include <iostream>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <pthread.h>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"

// Constructor: initialize member variables
Server::Server() : listen_sock(-1), stop_flag(false)
{
    // Initialize mutex
    if (pthread_mutex_init(&tables_mutex, nullptr) != 0) {
        throw std::runtime_error("Failed to initialize mutex");
    }
}

// Destructor: clean up resources
Server::~Server()
{
    // Destroy the mutex
    pthread_mutex_destroy(&tables_mutex);
    // Close the listening socket if it's open
    if (listen_sock != -1) {
        close(listen_sock);
    }
}

// Listen for incoming connections on the given port
void Server::listen(const std::string &port)
{
    listen_sock = Open_listenfd(port.c_str()); // Open the listen socket
    if (listen_sock < 0) {
        throw std::runtime_error("Failed to open listening socket");
    }
}

// Main server loop - accept incoming connections and spawn threads
void Server::server_loop()
{
    while (!stop_flag) {
        int client_fd = Accept(listen_sock, nullptr, nullptr);  // Block waiting for a connection
        if (client_fd < 0) {
            log_error("Failed to accept connection");
            continue; // Retry if accepting a connection fails
        }

        // Create a ClientConnection object to manage the new client
        ClientConnection *client = new ClientConnection(this, client_fd);
        
        // Start a worker thread to handle the client's interaction
        pthread_t thr_id;
        if (pthread_create(&thr_id, nullptr, client_worker, client) != 0) {
            this->log_error("Could not create client thread");
            delete client; // Clean up if thread creation fails
        }
    }
}

// Worker thread function for handling client interactions
void *Server::client_worker(void *arg)
{
    pthread_detach(pthread_self());  // Detach the thread to avoid needing pthread_join
    
    std::unique_ptr<ClientConnection> client(static_cast<ClientConnection *>(arg));
    
    try {
        // Handle client interaction (e.g., chat or request handling)
        client->chat_with_client();
    } catch (const std::exception &e) {
        log_error("Client worker error: " + std::string(e.what()));
    }
    
    return nullptr; // Thread exits here
}

// Log errors to standard error
void Server::log_error(const std::string &what)
{
    std::cerr << "Error: " << what << "\n";
}

// Helper function to find a table by name
Table *Server::find_table(const std::string &name)
{
    // Lock the tables mutex to ensure thread-safe access
    pthread_mutex_lock(&tables_mutex);
    
    auto it = tables.find(name);
    if (it != tables.end()) {
        pthread_mutex_unlock(&tables_mutex);  // Unlock before returning the table
        return it->second.get();              // Return the found table
    }
    
    pthread_mutex_unlock(&tables_mutex);  // Unlock if table not found
    return nullptr;  // Table not found
}

// Helper function to create a new table
void Server::create_table(const std::string &name)
{
    // Lock the tables mutex to ensure thread-safe access
    pthread_mutex_lock(&tables_mutex);
    
    if (tables.find(name) != tables.end()) {
        pthread_mutex_unlock(&tables_mutex);
        log_error("Table with this name already exists: " + name);
        return;  // Table already exists, don't create a new one
    }
    
    // Create the new table and add it to the map
    tables[name] = std::make_unique<Table>(name);
    pthread_mutex_unlock(&tables_mutex);  // Unlock after modifying the tables map
}

