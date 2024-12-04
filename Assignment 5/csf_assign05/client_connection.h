#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <set>
#include "message.h"
#include "csapp.h"
#include "value_stack.h"
#include "message_serialization.h"

class Server; // forward declaration
class Table; // forward declaration

class ClientConnection {
private:
  Server *m_server;
  int m_client_fd;
  rio_t m_fdbuf;
  bool in_transaction;
  ValueStack operand_stack;
  std::vector<std::string> m_table_names;

  //ADDED FUNCTIONALITY
  
  // Helper function to process a message from the client
  void process_request(const Message &request);

  // Helper function to handle exceptions and clean up resources
  void handle_exception(const std::exception &e);

  // Helper function to prepare response
  Message prepare_response(const Message &request);

  // Handler for LOGIN request
  void handle_login(const Message &request);

  // Handler for BYE request
  void handle_bye(const Message &request);

  // Handler for CREATE request
  void handle_create(const Message &request);

  // Handler for GET request
  void handle_get(const Message &request);

  // Handler for SET request
  void handle_set(const Message &request);

  // Handler for PUSH request
  void handle_push(const Message &request);

  // Handler for POP request
  void handle_pop(const Message &request);

  // Handler for TOP request
  void handle_top(const Message &request);

  // Handler for ADD request
  void handle_add(const Message &request);

  // Handler for SUB request
  void handle_sub(const Message &request);

  // Handler for MUL request
  void handle_mul(const Message &request);

  // Handler for DIV request
  void handle_div(const Message &request);

  // Handler for BEGIN request
  void handle_begin(const Message &request);

  // Handler for COMMIT request
  void handle_commit(const Message &request);


  // Helper function to manage locks on tables
  void lock_table(Table* table);
  void unlock_all_tables();

  // Helper function to send a message using rio_writen
  void send_response(const Message &msg);

  // Helper function to receive a message using rio_readlineb
  std::string receive_message(rio_t &rio);

  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );


  bool find_table(std::string table_name);

public:
  ClientConnection( Server *server, int client_fd );
  ~ClientConnection();

  void chat_with_client();

  // TODO: additional member functions
  // Added utility functions for transactions
    void begin_transaction();
    void commit_transaction();
    void rollback_transaction();
};

#endif // CLIENT_CONNECTION_H
