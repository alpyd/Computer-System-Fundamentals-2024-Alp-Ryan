#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
  , in_transaction (false) {
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection() {
  // Clean up any resources, close the client connection.
  close(m_client_fd);
}

// Helper function to send response using Rio_writen
void ClientConnection::send_response(const Message &msg) {
  std::string encoded_msg;
  MessageSerialization::encode(msg, encoded_msg);
  ssize_t n = rio_writen(m_client_fd, encoded_msg.c_str(), encoded_msg.length()); 
  if(n <= 0) {
    throw CommException("Error writing to client");
  }
}

// Helper function to receive a message using rio_readlineb
std::string ClientConnection::receive_message(rio_t &rio) {
  char buffer[MAXLINE];
  ssize_t n = rio_readlineb(&rio, buffer, MAXLINE);
  if (n <= 0) {
    throw CommException("Error reading from client");
  }
  // Null-terminate the response
  buffer[n] = '\0';  

  return std::string(buffer);
}

void ClientConnection::chat_with_client() {
  bool first_request = true;
  bool in_conversation = true;
  
  while (in_conversation) {
    try {
      // Process the message from the client and update in_conversation accordingly
      in_conversation = process_message_from_client(first_request);
      first_request = false;
    //Request handler functions nested in process_request will send response to client
    } catch (const InvalidMessage &e) {
      // Invalid message, respond with ERROR and terminate the connection
      handle_invalid_message(e.what());

      // Exit the loop and end the connection
      break;  
    } 
    catch (const CommException &e) {
      // Communication error, respond with ERROR and terminate the connection
      handle_comm_exception(e.what());

      // Exit the loop and end the connection
      break;  
    } catch (const OperationException &e) {
      // Handle OperationException and don't end the connection
      handle_operation_exception();
    } catch (const FailedTransaction &e) {
      // Handle FailedTransaction and don't end the connection
      handle_failed_transaction(e.what());

    } catch (const std::exception &e) {
      // Handle other unexpected exceptions
      handle_unexpected_exception(e.what());

      // Exit the loop and end the connection
      break;  
    }
  }
}

bool ClientConnection::process_message_from_client(const bool first_request) {
  Message request;

  // Step 1: Read a message from the client
  std::string encoded_msg = receive_message(m_fdbuf); 
  MessageSerialization::decode(encoded_msg, request);
  
  if(!first_request && request.get_message_type() == MessageType::LOGIN) { 
    //Ensure only 1 starting LOGIN request
    throw InvalidMessage("No duplicate LOGIN's allowed");
  }

  if (first_request && request.get_message_type() != MessageType::LOGIN) { 
    // Must start with a LOGIN request
    throw InvalidMessage("First message must be a LOGIN request");
  }

  // Step 2: Process the request, send response
  process_request(request);
  
  return request.get_message_type() != MessageType::BYE;
}

void ClientConnection::handle_invalid_message(std::string error_msg) {
  // Handle case of ongoing transaction to safely rollback changes
  if(in_transaction) {
    // Roll back changes and end transaction state
    rollback_changes();
    in_transaction = false;
  }

  std::cerr << "Invalid message received from client: " << error_msg << std::endl;
  Message msg(MessageType::ERROR);

  // Add the error message as an argument
  msg.push_arg(error_msg); 
  send_response(msg);
}

void ClientConnection::handle_comm_exception(std::string error_msg) {
  // Handle case of ongoing transaction to safely rollback changes
  if(in_transaction) {
    // Roll back changes and end transaction state
    rollback_changes();
    in_transaction = false;
  }

  std::cerr << "Communication error with client: " << error_msg << std::endl;
  Message msg(MessageType::ERROR);

  // Add the error message as an argument
  msg.push_arg(error_msg); 
  send_response(msg);
}

void ClientConnection::handle_operation_exception() {
  // Handle case of ongoing transaction to safely rollback changes
  if(in_transaction) {
    // Roll back changes and end transaction state
    rollback_changes();
    in_transaction = false;
  }
}

void ClientConnection::handle_failed_transaction(std::string error_msg) {
  // Roll back changes and end transaction state
  rollback_changes();
  in_transaction = false;

  std::cerr << "Transaction failed: " << error_msg << std::endl;
  Message msg(MessageType::ERROR);
  // Add the error message as an argument
  msg.push_arg(error_msg); 
  send_response(msg);
}

void ClientConnection::handle_unexpected_exception(std::string error_msg) {
  if(in_transaction) {
    // Roll back changes and end transaction state
    rollback_changes();
    in_transaction = false;
  }

  std::cerr << "Unexpected error: " << error_msg << std::endl;
  Message msg(MessageType::ERROR);

  // Add the error message as an argument
  msg.push_arg(error_msg); 
  send_response(msg);
}

void ClientConnection::rollback_changes() {
  // For each table_name that was changed in transaction, rollback changes and unlock to prevent deadlock
  for (std::string name : m_table_names) {
    Table* table = m_server->find_table(name);
    if (table != nullptr) {
      table->rollback_changes();
      table->unlock();
    }
  }
}

void ClientConnection::process_request(const Message &request) {
  // Handle different types of requests based on message type
  switch (request.get_message_type()) {
    case MessageType::LOGIN:
      handle_login(request);
      break;
    case MessageType::CREATE:
      handle_create(request);
      break;
    case MessageType::PUSH:
      handle_push(request);
      break;
    case MessageType::POP:
      handle_pop(request);
      break;
    case MessageType::TOP:
      handle_top(request);
      break;
    case MessageType::SET:
      handle_set(request);
      break;
    case MessageType::GET:
      handle_get(request);
      break;
    case MessageType::ADD:
      handle_add(request);
      break;
    case MessageType::MUL:
      handle_mul(request);
      break;
    case MessageType::SUB:
      handle_sub(request);
      break;
    case MessageType::DIV:
      handle_div(request);
      break;
    case MessageType::BEGIN:
      handle_begin(request);
      break;
    case MessageType::COMMIT:
      handle_commit(request);
      break;
    case MessageType::BYE:
      handle_bye(request);
      break;
    default:
      // Unknown message type, invalid request
      throw InvalidMessage("Unknown message type");  
  }
}

void ClientConnection::handle_begin(const Message &request) {
  // Validate that the BEGIN request is valid
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid BEGIN request format");
  }
  
  if (in_transaction) {
    throw FailedTransaction("A transaction is already ongoing");
  }

  in_transaction = true;
  send_response(MessageType::OK);
}

void ClientConnection::handle_commit(const Message &request) {
  // Validate that the COMMIT request is valid
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid COMMIT request format");
  }

  if (!in_transaction) {
    throw InvalidMessage("A transaction has not been started");
  }

  // Commit changes and unlock mutex for all of the changed tables
  for (std::string name : m_table_names) {
    Table* table = m_server->find_table(name);
    if (table != nullptr) {
      table->commit_changes();
      table->unlock();
    }
  }

  // Reset variables now that transaction is done
  m_table_names.clear();
  in_transaction = false;

  // Send OK response
  send_response(MessageType::OK);
}

void ClientConnection::handle_login(const Message &request) {
  // Validate that the username is a valid identifier
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid username format");
  }

  // Send response back indicating successful login
  send_response(MessageType::OK);
}

void ClientConnection::handle_create(const Message &request) {
  // Ensure input is formatted properly
  if (request.get_num_args() != 1) {
    throw InvalidMessage("CREATE requires exactly 1 argument (table name)");
  }

  std::string table_name = request.get_table();
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid table name format");
  }

  //Create table in server's collection of tables
  m_server->create_table(table_name);

  send_response(MessageType::OK);
}

void ClientConnection::handle_get(const Message &request) {
  if (request.get_num_args() != 2) {
    throw InvalidMessage("GET requires exactly 2 arguments (table and key)");
  }

  // Isolate the table and key to get data from
  std::string table_name = request.get_table();
  std::string key = request.get_key();

  if (!request.is_valid()) {
    throw InvalidMessage("Invalid table name or key format");
  }

  //Get the table from server, lock table, update key's value, unlock table, proceed to push onto stack
  if (in_transaction) {
    transaction_get(table_name, key);
  } else {
    Table* requested = m_server->find_table(table_name);
    requested->lock();
    std::string value = requested->get(key);
    requested->unlock();  

    // Push the retrieved value onto the operand stack
    operand_stack.push(value);
  }
  send_response(MessageType::OK);
}

void ClientConnection::transaction_get(const std::string& table_name, const std::string& key) {
  //Get the table from server, lock table, update key's value, unlock table, proceed to push onto stack
  Table* requested = m_server->find_table(table_name);
  if (!find_table(table_name)) {
    if(!requested->trylock()) {
      throw FailedTransaction("Error: Could not lock the table");
    }
    // Add the table name to the larger vector of table names
    m_table_names.push_back(table_name);
  }
  if(!requested->has_key(key)) {
    throw InvalidMessage("Error: Provided key does not exist");
  }
  operand_stack.push(requested->get(key));
}

void ClientConnection::handle_set(const Message &request) {
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid SET request format");
  }

  try {
    // Ensure the operand stack is not empty, as we need a value to set
    if (operand_stack.is_empty()) {
      throw OperationException("Operand stack is empty, cannot set value");
    }

    // Isolate the table and key to set data
    std::string table_name = request.get_table();
    std::string key = request.get_key();

    // Get the value from the top of stack to set and pop it from stack afterwards
    std::string value_to_set = operand_stack.get_top();
    operand_stack.pop();  

    // Get table from server, lock it, update the key's value, unlock
    if (in_transaction) {
      transaction_set(table_name, key, value_to_set);
    } else {
      normal_set(table_name, key, value_to_set);
    }
    send_response(MessageType::OK);
  } catch (const OperationException &e) {
    send_failed_response(e.what());
  } catch (const std::exception &e) {
    send_failed_response(e.what());
  }
}

void ClientConnection::transaction_set(const std::string& table_name, const std::string& key, const std::string& value_to_set) {
  // Get the requested table and throw InvalidMessage if requested table not present
  Table* requested = m_server->find_table(table_name);
  if (requested == nullptr) {
    throw InvalidMessage("Error: Specified table is not present");
  }

  // Lock the table and add the table name to the global vector of table names
  if (!find_table(table_name)) {
    if(!requested->trylock()) {
      throw FailedTransaction("Error: Could not lock the table");
    }
    m_table_names.push_back(table_name);
  }

  // Set the value of the requested table at the specified key
  requested->set(key, value_to_set);
}

void ClientConnection::normal_set(const std::string& table_name, const std::string& key, const std::string& value_to_set) {
  // Get the requested table and throw InvalidMessage if requested table not present
  Table* requested = m_server->find_table(table_name);
  if (requested == nullptr) {
    throw InvalidMessage("Error: Specified table is not present");
  }

  // Lock the table, set the value of the requested table at the specified key, and unlock table to ensure no deadlocks
  requested->lock();
  requested->set(key, value_to_set);
  requested->unlock();
}

bool ClientConnection::find_table(std::string table_name) {
  for (std::string name: m_table_names) {
    if (name == table_name) {
      return true;
    }
  }
  return false;
}

void ClientConnection::handle_bye(const Message &request) {
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid BYE request format.");
  }
  send_response(MessageType::OK);
}

void ClientConnection::handle_push(const Message &request) {
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid PUSH request format.");
  }
  std::string value = request.get_value();
  if (value.empty()) {
    throw InvalidMessage("Value cannot be empty.");
  }
  // Push the value onto the operand stack
  operand_stack.push(value);  

  // Respond with OK
  send_response(MessageType::OK);  
}

void ClientConnection::handle_pop(const Message &request) {
  // Ensure the request is valid
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid format of POP");
  }

  try {
    // Pop the value from the operand stack
    operand_stack.pop();  

    // Respond with OK
    send_response(MessageType::OK);  
  } catch (const OperationException &e) {
    send_failed_response(e.what());
  } catch (const std::exception &e) {
    send_failed_response(e.what());
  }
}

void ClientConnection::handle_top(const Message &request) {
  // Ensure the request has no arguments
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid format of TOP");
  }

  try {
    // Get the top value from the stack
    std::string top_value = operand_stack.get_top();  

    if (!top_value.empty()) {
      Message msg(MessageType::DATA);
      // Add the top value as an argument
      msg.push_arg(top_value); 
      send_response(msg);
    } else {
      throw OperationException("Operand stack is empty.");
    }
  } catch (const OperationException &e) {
    send_failed_response(e.what());
  } catch (const std::exception &e) {
    send_failed_response(e.what());
  }
}

void ClientConnection::handle_add(const Message &request) {
  // Ensure the request has no arguments
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid format of ADD");
  }

  try {
    // Get second operand and pop from the stack
    std::string operand2 = operand_stack.get_top();  
    operand_stack.pop();  

    // Get first operand and pop from the stack
    std::string operand1 = operand_stack.get_top();  
    operand_stack.pop();  

    // Perform the addition (assuming operands are integers)
    int result = std::stoi(operand1) + std::stoi(operand2);
    
    // Push the result back onto the stack
    operand_stack.push(std::to_string(result));  

    // Respond with OK
    send_response(MessageType::OK);  
  } catch (const OperationException &e) {
    send_failed_response(e.what());
  } catch (const std::exception &e) {
    send_failed_response(e.what());
  }
}

void ClientConnection::handle_sub(const Message &request) {
  // Ensure the request has no arguments
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid format of SUB");
  }

  try {
    // Get second operand and pop from the stack
    std::string operand2 = operand_stack.get_top(); 
    operand_stack.pop();  

    // Get first operand and pop from the stack
    std::string operand1 = operand_stack.get_top();  
    operand_stack.pop();  

    // Perform the subtraction (assuming operands are integers)
    int result = std::stoi(operand1) - std::stoi(operand2);

    // Push the result back onto the stack
    operand_stack.push(std::to_string(result));  

    // Respond with OK
    send_response(MessageType::OK);  
  } catch (const OperationException &e) {
    send_failed_response(e.what());
  } catch (const std::exception &e) {
    send_failed_response(e.what());
  }
}

void ClientConnection::handle_mul(const Message &request) {
  // Ensure the request has no arguments
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid format of MUL");
  }

  try {
    // Get the second operand from stack and pop it from stack
    std::string operand2 = operand_stack.get_top();
    operand_stack.pop(); 

    // Get the first operand from the stack and pop it from stack
    std::string operand1 = operand_stack.get_top();
    operand_stack.pop();

    // Perform the multiplication (assuming operands are integers)
    int result = std::stoi(operand1) * std::stoi(operand2);

    // Push the result back onto the stack
    operand_stack.push(std::to_string(result));

    // Respond with OK if everything is successful
    send_response(MessageType::OK);
  } catch (const OperationException &e) {
    send_failed_response(e.what());
  } catch (const std::exception &e) {
    send_failed_response(e.what());
  }
}

void ClientConnection::handle_div(const Message &request) {
  // Ensure the request has no arguments
  if (!request.is_valid()) {
    throw InvalidMessage("Invalid format of DIV");
  }

  try {
    // Get second operand and pop from the stack
    std::string operand2 = operand_stack.get_top(); 
    operand_stack.pop();

    // Get first operand and pop from the stack
    std::string operand1 = operand_stack.get_top();  
    operand_stack.pop(); 

    // Throw error if attempting to divide by 0
    if (std::stoi(operand2) == 0) {
      throw OperationException("Division by zero.");
    }

    // Perform the division (assuming operands are integers)
    int result = std::stoi(operand1) / std::stoi(operand2);

    // Push the result back onto the stack
    operand_stack.push(std::to_string(result));  

    // Respond with OK
    send_response(MessageType::OK);  
  } catch (const OperationException &e) {
    send_failed_response(e.what());
  } catch (const std::exception &e) {
    send_failed_response(e.what());
  }
}

void ClientConnection::send_failed_response(std::string error_msg) {
  // Create a FAILED MessageType
  Message msg(MessageType::FAILED);
  
  // Add the error message as an argument
  msg.push_arg(error_msg); 

  // Send the Failed Message Response
  send_response(msg);
}

 
