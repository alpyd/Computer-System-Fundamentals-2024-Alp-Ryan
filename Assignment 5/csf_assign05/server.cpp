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
Server::Server() : listen_sock(-1), stop_flag(false) {
  // Initialize mutex
  if (pthread_mutex_init(&tables_mutex, nullptr) != 0) {
    throw std::runtime_error("Failed to initialize mutex");
  }
}

// Destructor: clean up resources
Server::~Server() {
  // Destroy the mutex
  pthread_mutex_destroy(&tables_mutex);
  // Close the listening socket if it's open
  if (listen_sock != -1) {
    close(listen_sock);
  }
}

// Listen for incoming connections on the given port
void Server::listen(const std::string &port) {
  listen_sock = open_listenfd(port.c_str()); // Open the listen socket
  if (listen_sock < 0) {
    throw std::runtime_error("Failed to open listening socket");
  }
}

// Main server loop - accept incoming connections and spawn threads
void Server::server_loop() {
  while (!stop_flag) {
    int client_fd = accept(listen_sock, nullptr, nullptr);  

    // Block waiting for a connection
    if (client_fd < 0) {
      log_error("Failed to accept connection");
      // Retry if accepting a connection fails
      continue; 
    }

    // Create a ClientConnection object to manage the new client
    ClientConnection *client = new ClientConnection(this, client_fd);
    
    // Start a worker thread to handle the client's interaction
    pthread_t thr_id;
    if (pthread_create(&thr_id, nullptr, client_worker, client) != 0) {
      this->log_error("Could not create client thread");

      // Clean up if thread creation fails
      delete client; 
    }
  }
}

// Worker thread function for handling client interactions
void *Server::client_worker(void *arg) {

  // Detach the thread to avoid needing pthread_join
  pthread_detach(pthread_self());  
  
  std::unique_ptr<ClientConnection> client(static_cast<ClientConnection *>(arg));
  
  try {
    // Handle client interaction (e.g., chat or request handling)
    client->chat_with_client();
  } catch (const std::exception &e) {
    std::cerr << "Client worker error: " << std::string(e.what()) << std::endl;
  }
  
  // Thread exits here
  return nullptr; 
}

// Log errors to standard error
void Server::log_error(const std::string &what) {
  std::cerr << "Error: " << what << "\n";
}

// Helper function to find a table by name
Table *Server::find_table(const std::string &name) {
  // Lock the tables mutex to ensure thread-safe access
  pthread_mutex_lock(&tables_mutex);
  
  auto it = tables.find(name);
  if (it != tables.end()) {
    // Unlock before returning the table
    pthread_mutex_unlock(&tables_mutex);  

    // Return the found table
    return it->second.get();              
  }
  
  // Unlock if table not found
  pthread_mutex_unlock(&tables_mutex);  

  // Table not found
  return nullptr;  
}

// Helper function to create a new table
void Server::create_table(const std::string &name) {
  // Lock the tables mutex to ensure thread-safe access
  pthread_mutex_lock(&tables_mutex);
  
  if (tables.find(name) != tables.end()) {
    pthread_mutex_unlock(&tables_mutex);
    log_error("Table with this name already exists: " + name);
    // Table already exists, don't create a new one
    return;  
  }
  
  // Create the new table and add it to the map
  tables[name] = std::make_unique<Table>(name);

  // Unlock after modifying the tables map
  pthread_mutex_unlock(&tables_mutex);  
}

