#include <set>
#include <map>
#include <regex>
#include <cassert>
#include "message.h"

Message::Message()
  : m_message_type(MessageType::NONE)
{
}

Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}

Message::Message( const Message &other )
  : m_message_type( other.m_message_type )
  , m_args( other.m_args )
{
}

Message::~Message()
{
}

Message &Message::operator=( const Message &rhs ) {
  if (this != &rhs) {
    m_message_type = rhs.m_message_type;
    m_args = rhs.m_args;
  }
  return *this;
}

MessageType Message::get_message_type() const {
  return m_message_type;
}

void Message::set_message_type(MessageType message_type) {
  m_message_type = message_type;
}

std::string Message::get_username() const {
  // Return the username from the message for LOGIN
 if (m_message_type == MessageType::LOGIN && !m_args.empty()) {
    return m_args[0];
  }
  return "";
}

std::string Message::get_table() const {
  // Return the table from the message
  if ((m_message_type == MessageType::CREATE || m_message_type == MessageType::SET || m_message_type == MessageType::GET) && !m_args.empty()) {
    return m_args[0];
  }
  return "";
}

std::string Message::get_key() const {
  // Return the key from the message
  if ((m_message_type == MessageType::SET || m_message_type == MessageType::GET) && m_args.size() > 1) {
    return m_args[1];
  }
  return "";
}

std::string Message::get_value() const {
  // Return the value of the message
  if ((m_message_type == MessageType::SET && m_args.size() > 2) ||
    (m_message_type == MessageType::PUSH && !m_args.empty()) ||
    (m_message_type == MessageType::DATA && !m_args.empty())) {
    return m_args.back();
  }
  return "";
}

std::string Message::get_quoted_text() const {
  if ((m_message_type == MessageType::FAILED || m_message_type == MessageType::ERROR) && !m_args.empty()) {
    return m_args[0];
  }
  return "";
}

void Message::push_arg( const std::string &arg ) {
  m_args.push_back( arg );
}

//Checks if a string is a valid identifier
bool is_valid_identifier(const std::string& str) {
  if (str.empty() || !std::isalpha(str[0])) {
    return false; // identifiers must start with a letter
  }
  for (char ch : str) {
    if (!std::isalnum(ch) && ch != '_') {
      return false; // only letters, digits, and underscores are allowed
    }
  }
  return true;
}

//Checks if a string is a valid value (non-whitespace characters only)
bool is_valid_value(const std::string& str) {
  return !str.empty() && str.find_first_of(" \t\n") == std::string::npos;
}

//Checks if a string is valid plain text (for errors)
bool is_valid_plain_text(const std::string& str) {
  return !str.empty();
}

bool Message::is_valid() const {
    // Check the message type and validate
  switch (m_message_type) {
    case MessageType::LOGIN:
      // LOGIN should have exactly 1 identifier argument
      return get_num_args() == 1 && is_valid_identifier(m_args[0]);

    case MessageType::CREATE:
      // CREATE should have exactly 1 identifier argument
      return get_num_args() == 1 && is_valid_identifier(m_args[0]);

    case MessageType::PUSH:
      // PUSH should have exactly 1 valid value argument
      return get_num_args() == 1 && is_valid_value(m_args[0]);

    case MessageType::POP:
    case MessageType::TOP:
    case MessageType::ADD:
    case MessageType::SUB:
    case MessageType::MUL:
    case MessageType::DIV:
    case MessageType::BEGIN:
    case MessageType::COMMIT:
    case MessageType::BYE:
    case MessageType::OK:
      // POP, arithmetic operations, BYE, and OK should have no arguments
      return get_num_args() == 0;

    case MessageType::SET:
      // SET should have exactly 2 identifier arguments (table and key)
      return get_num_args() == 2 &&is_valid_identifier(m_args[0]) && is_valid_identifier(m_args[1]);

    case MessageType::GET:
      // GET should have exactly 2 identifier arguments (table and key)
      return get_num_args() == 2 && is_valid_identifier(m_args[0]) && is_valid_identifier(m_args[1]);

    case MessageType::FAILED:
    case MessageType::ERROR:
      // FAILED and ERROR should have exactly 1 plain text argument (stripped quotes)
      return get_num_args() == 1 && is_valid_plain_text(m_args[0]);

    case MessageType::DATA:
      // DATA should have exactly 1 valid value argument
      return get_num_args() == 1 && is_valid_value(m_args[0]);

    default: // If the message type is unknown or doesn't conform to any known type
      return false;
  }
}