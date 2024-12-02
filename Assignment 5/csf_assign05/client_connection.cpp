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
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
  // TODO: implement
  // Clean up any resources, close the client connection.
  close(m_client_fd);
}

// Helper function to send response using Rio_writen
void ClientConnection::send_response(const Message &msg) {
  std::string encoded_msg;
  MessageSerialization::encode(msg, encoded_msg);
  Rio_writen(m_client_fd, encoded_msg.c_str(), encoded_msg.length());
}

// Helper function to receive a message using rio_readlineb
std::string ClientConnection::receive_message(rio_t &rio) {
  char buffer[MAXLINE];
  ssize_t n = Rio_readlineb(&rio, buffer, MAXLINE);
  if (n == 0) {
    std::cerr << "Error: Server closed the connection unexpectedly\n";
    exit(1);
  }
  buffer[n] = '\0';  // Null-terminate the response
  return std::string(buffer);
}

void ClientConnection::chat_with_client()
{
    Message request;
    Message response;
    bool first_request = true;
    
    while (true) {
        try {
            // Step 1: Read a message from the client
            std::string encoded_msg = receive_message(m_fdbuf); 
            MessageSerialization::decode(encoded_msg, request);

            if (first_request && request.get_message_type() != MessageType::LOGIN) { //Must start with a LOGIN request
              throw InvalidMessage("First message must be a LOGIN request");
            }
            first_request = false;
            
            // Step 2: Process the request, send response
            process_request(request);
            
            if(request.get_message_type() == MessageType::BYE) {
              break;
            }
            //Request handler functions nested in process_request will send response to client

        } 
        catch (const InvalidMessage &e) {
            // Invalid message, respond with ERROR and terminate the connection
            std::cerr << "Invalid message received from client: " << e.what() << std::endl;
            send_response(MessageType::ERROR);  // Send the error response to the client
            //close(m_fdbuf.rio_fd);  
            break;  // Exit the loop and end the connection
        } 
        catch (const CommException &e) {
            // Communication error, respond with ERROR and terminate the connection
            std::cerr << "Communication error with client: " << e.what() << std::endl;
            send_response(MessageType::ERROR);  // Send the error response to the client
            //close(m_fdbuf.rio_fd);  
            break;  // Exit the loop and end the connection
        } 
        catch (const std::exception &e) {
            // Handle other unexpected exceptions
            std::cerr << "Unexpected error: " << e.what() << std::endl;
            send_response(MessageType::ERROR);  // Send the error response to the client
            //close(m_fdbuf.rio_fd);  
            break;  // Exit the loop and end the connection
        }
    }
}

void ClientConnection::process_request(const Message &request)
{
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
            //handle_begin();
            break;
        case MessageType::COMMIT:
            //handle_commit();
            break;
        case MessageType::BYE:
            handle_bye(request);
            break;
        default:
            throw InvalidMessage("Unknown message type");  // Unknown message type, invalid request
    }
}

void ClientConnection::handle_login(const Message &request)
{
        
        // Validate that the username is a valid identifier
        if (!request.is_valid()) {
            throw InvalidMessage("Invalid username format");
        }

        // Send response back indicating successful login
        send_response(MessageType::OK);
}

void ClientConnection::handle_create(const Message &request)
{
   
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

void ClientConnection::handle_get(const Message &request)
{
    if (request.get_num_args() != 2) {
        throw InvalidMessage("GET requires exactly 2 arguments (table and key)");
    }

    std::string table_name = request.get_table();
    std::string key = request.get_key();

    if (!request.is_valid()) {
        throw InvalidMessage("Invalid table name or key format");
    }

    //Get the table from server, lock table, update key's value, unlock table, proceed to push onto stack
    Table* requested = m_server->find_table(table_name);
    requested->lock();
    std::string value = requested->get(key);
    requested->unlock();  

    // Push the retrieved value onto the operand stack
    operand_stack.push(value);
    send_response(MessageType::OK);
}

void ClientConnection::handle_set(const Message &request)
{
    
  if (!request.is_valid()) {
      throw InvalidMessage("Invalid SET request format");
  }

  try {
        // Ensure the operand stack is not empty, as we need a value to set
        if (operand_stack.is_empty()) {
            throw OperationException("Operand stack is empty, cannot set value");
        }

        std::string table_name = request.get_table();
        std::string key = request.get_key();
      
        std::string value_to_set = operand_stack.get_top();
        operand_stack.pop();  // Pop the value from the stack

        // Get table from server, lock it, update the key's value, unlock
        Table* requested = m_server->find_table(table_name);
        requested->lock();
        requested->set(key, value_to_set);
        requested->unlock(); 

        send_response(MessageType::OK);

    } catch (const OperationException &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    } catch (const std::exception &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    }
}

void ClientConnection::handle_bye(const Message &request) {
  
if (!request.is_valid()) {
  throw InvalidMessage("Invalid BYE request format.");
}

send_response(MessageType::OK);

}

void ClientConnection::handle_push(const Message &request)
{
    
  if (!request.is_valid()) {
      throw InvalidMessage("Invalid PUSH request format.");
  }

  std::string value = request.get_value();
  if (value.empty()) {
      throw InvalidMessage("Value cannot be empty.");
  }

  operand_stack.push(value);  // Push the value onto the operand stack

  send_response(MessageType::OK);  // Respond with OK
}

void ClientConnection::handle_pop(const Message &request)
{
    // Ensure the request has no arguments
    if (request.get_num_args() != 0) {
        throw InvalidMessage("No arguments allowed for POP");
    }

    try {
        operand_stack.pop();  // Pop the value from the operand stack
        send_response(MessageType::OK);  // Respond with OK
    } 
    
    catch (const OperationException &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    } catch (const std::exception &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    }
}

void ClientConnection::handle_top(const Message &request)
{
  // Ensure the request has no arguments
    if (request.get_num_args() != 0) {
        throw InvalidMessage("No arguments allowed for TOP");
    }

    try {
        std::string top_value = operand_stack.get_top();  // Get the top value from the stack
        if (!top_value.empty()) {
          Message msg(MessageType::DATA);
          msg.push_arg(top_value); // Add the top value as an argument
          send_response(msg);
        } else {
            throw OperationException("Operand stack is empty.");
        }
    } catch (const OperationException &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    } catch (const std::exception &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    }
}

void ClientConnection::handle_add(const Message &request)
{
  // Ensure the request has no arguments
    if (request.get_num_args() != 0) {
        throw InvalidMessage("No arguments allowed for ADD");
    }

    try {
        std::string operand2 = operand_stack.get_top();  // Get second operand
        operand_stack.pop();  // Pop operand2 from the stack

        std::string operand1 = operand_stack.get_top();  // Get first operand
        operand_stack.pop();  // Pop operand1 from the stack

        // Perform the addition (assuming operands are integers)
        int result = std::stoi(operand1) + std::stoi(operand2);
        operand_stack.push(std::to_string(result));  // Push the result back onto the stack

        send_response(MessageType::OK);  // Respond with OK
    } catch (const OperationException &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    } catch (const std::exception &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    }
}

void ClientConnection::handle_sub(const Message &request)
{
  // Ensure the request has no arguments
    if (request.get_num_args() != 0) {
        throw InvalidMessage("No arguments allowed for SUB");
    }

    try {
        std::string operand2 = operand_stack.get_top();  // Get second operand
        operand_stack.pop();  // Pop operand2 from the stack

        std::string operand1 = operand_stack.get_top();  // Get first operand
        operand_stack.pop();  // Pop operand1 from the stack

        // Perform the subtraction (assuming operands are integers)
        int result = std::stoi(operand1) - std::stoi(operand2);
        operand_stack.push(std::to_string(result));  // Push the result back onto the stack

        send_response(MessageType::OK);  // Respond with OK
    } catch (const OperationException &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    } catch (const std::exception &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    }
}

void ClientConnection::handle_mul(const Message &request)
{
  // Ensure the request has no arguments
    if (request.get_num_args() != 0) {
        throw InvalidMessage("No arguments allowed for MUL");
    }

    try {
        // Get the second operand from the stack
        std::string operand2 = operand_stack.get_top();
        operand_stack.pop();  // Pop operand2 from the stack

        // Get the first operand from the stack
        std::string operand1 = operand_stack.get_top();
        operand_stack.pop();  // Pop operand1 from the stack

        // Perform the multiplication (assuming operands are integers)
        int result = std::stoi(operand1) * std::stoi(operand2);

        // Push the result back onto the stack
        operand_stack.push(std::to_string(result));

        // Respond with OK if everything is successful
        send_response(MessageType::OK);
    } catch (const OperationException &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    } catch (const std::exception &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    }
}

void ClientConnection::handle_div(const Message &request)
{
  // Ensure the request has no arguments
    if (request.get_num_args() != 0) {
        throw InvalidMessage("No arguments allowed for DIV");
    }

    try {
        std::string operand2 = operand_stack.get_top();  // Get second operand
        operand_stack.pop();  // Pop operand2 from the stack

        std::string operand1 = operand_stack.get_top();  // Get first operand
        operand_stack.pop();  // Pop operand1 from the stack

        // Perform the division (assuming operands are integers)
        int operand2_val = std::stoi(operand2);
        if (operand2_val == 0) {
            throw OperationException("Division by zero.");
        }

        int result = std::stoi(operand1) / operand2_val;
        operand_stack.push(std::to_string(result));  // Push the result back onto the stack

        send_response(MessageType::OK);  // Respond with OK
    } catch (const OperationException &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    } catch (const std::exception &e) {
        Message msg(MessageType::FAILED);
        msg.push_arg(e.what()); // Add the error message as an argument
        send_response(msg);
    }
}

 
