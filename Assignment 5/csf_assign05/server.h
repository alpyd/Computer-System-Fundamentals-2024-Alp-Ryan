#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <string>
#include <pthread.h>
#include <memory>
#include "table.h"
#include "client_connection.h"

class Server {
private:
  // TODO: add member variables
  int listen_sock;                                     // Listening socket
  bool stop_flag;                                     // Flag to stop the server loop
  std::map<std::string, std::unique_ptr<Table>> tables; // Map of table names to Table objects
  pthread_mutex_t tables_mutex;                      // Mutex to protect access to the `tables` map
  
  // copy constructor and assignment operator are prohibited
  Server( const Server & );
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  void listen( const std::string &port );
  void server_loop();

  static void *client_worker( void *arg );

  void log_error( const std::string &what );

  // TODO: add member functions
  void handle_client_connection(int client_fd);
  void create_table(const std::string &name);
  Table* find_table(const std::string &name);         // Helper to find a table by name

  // Some suggested member functions:
/*
  void create_table( const std::string &name );
  Table *find_table( const std::string &name );
  void log_error( const std::string &what );
*/
};


#endif // SERVER_H
