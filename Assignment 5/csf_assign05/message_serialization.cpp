#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"


//Bidirectional mappings of Message object to String for use in encode/decode
namespace {
	std::string message_type_to_string(MessageType type) {
		static const std::map<MessageType, std::string> type_to_str = {
			{MessageType::LOGIN, "LOGIN"}, {MessageType::CREATE, "CREATE"},
			{MessageType::PUSH, "PUSH"}, {MessageType::POP, "POP"},
			{MessageType::TOP, "TOP"}, {MessageType::SET, "SET"},
			{MessageType::GET, "GET"}, {MessageType::ADD, "ADD"},
			{MessageType::SUB, "SUB"}, {MessageType::MUL, "MUL"},
			{MessageType::DIV, "DIV"}, {MessageType::BEGIN, "BEGIN"},
			{MessageType::COMMIT, "COMMIT"}, {MessageType::BYE, "BYE"},
			{MessageType::OK, "OK"}, {MessageType::FAILED, "FAILED"},
			{MessageType::ERROR, "ERROR"}, {MessageType::DATA, "DATA"},
		};
		return type_to_str.at(type);
	}

	MessageType string_to_message_type(const std::string &str) {
		static const std::map<std::string, MessageType> str_to_type = {
			{"LOGIN", MessageType::LOGIN}, {"CREATE", MessageType::CREATE},
			{"PUSH", MessageType::PUSH}, {"POP", MessageType::POP},
			{"TOP", MessageType::TOP}, {"SET", MessageType::SET},
			{"GET", MessageType::GET}, {"ADD", MessageType::ADD},
			{"SUB", MessageType::SUB}, {"MUL", MessageType::MUL},
			{"DIV", MessageType::DIV}, {"BEGIN", MessageType::BEGIN},
			{"COMMIT", MessageType::COMMIT}, {"BYE", MessageType::BYE},
			{"OK", MessageType::OK}, {"FAILED", MessageType::FAILED},
			{"ERROR", MessageType::ERROR}, {"DATA", MessageType::DATA},
		};
		auto it = str_to_type.find(str);
		return (it != str_to_type.end()) ? it->second : MessageType::NONE;
	}

	//Helper function to parse quoted text in string into FAILED/ERROR Message objects
	std::string parse_quoted_text(std::istringstream &iss) {
		std::string token;

		//If quote doesn't start quoted text, invalid message
		if (!(iss >> std::ws >> token) || token[0] != '"') throw InvalidMessage("Expected quoted text.");

		std::string quoted_text = token.substr(1);
		std::string part;
		bool quote_found = false;
		while (iss >> part) {
			if (!quote_found) { //add blanks if no quote yet
				quoted_text += " ";
			} 
			quoted_text += part; //add the next token from iss

			if (part.back() == '"') {
				quoted_text.pop_back();  // Remove closing quote
				quote_found = true;
				break;
			}
		}
		if (!quote_found) throw InvalidMessage("Unmatched quote in quoted text.");
		return quoted_text;
	}

	// Helper function for decoding string arguments into appropriate Message object arguments
	void parse_message_arguments(std::istringstream& iss, Message& msg, MessageType message_type) {
		std::string arg1, arg2;

		switch (message_type) {
			case MessageType::LOGIN: //1 argument: username
				iss >> arg1;
				msg.push_arg(arg1);
				break;

			case MessageType::CREATE: //1 argument: table name
				iss >> arg1;
				msg.push_arg(arg1);
				break;

			case MessageType::PUSH: //1 argument: value
				iss >> arg1;
				msg.push_arg(arg1);
				break;

			case MessageType::POP: // No arguments expected for these types
			case MessageType::TOP:
			case MessageType::BEGIN:
			case MessageType::COMMIT:
			case MessageType::BYE:
			case MessageType::OK:
			break;

			case MessageType::SET: //2 arguments: table, key
				iss >> arg1 >> arg2;
				msg.push_arg(arg1);
				msg.push_arg(arg2);
				break;

			case MessageType::GET: //2 arguments: table, key
				iss >> arg1 >> arg2;
				msg.push_arg(arg1);
				msg.push_arg(arg2);
				break;

			case MessageType::ADD: //No arguments to parse, operations rely on operand stack 
			case MessageType::SUB:
			case MessageType::MUL:
			case MessageType::DIV:
				break;

			case MessageType::FAILED: //1 argument: quoted text
			case MessageType::ERROR:
				msg.push_arg(parse_quoted_text(iss));
				break;

			case MessageType::DATA: //1 argument: value
				iss >> arg1;
				msg.push_arg(arg1);
				break;

			default:
				throw InvalidMessage("Unsupported message type for parsing.");
		}
		if (!(iss >> std::ws).eof()) { // Check for extraneous input
			throw InvalidMessage("Too many arguments for the command.");
		}
	}
}

//Convert a Message object into an encoded string version
void MessageSerialization::encode(const Message &msg, std::string &encoded_msg) {
	std::ostringstream oss;
	oss << message_type_to_string(msg.get_message_type()); //add message type to string
	for (unsigned i = 0; i < msg.get_num_args(); ++i) { //add all arguments of message to string
		oss << ' ' << msg.get_arg(i);
	}
	oss << '\n';
	encoded_msg = oss.str();
	if (encoded_msg.size() > Message::MAX_ENCODED_LEN) { //check for valid size
		throw InvalidMessage("Encoded message exceeds maximum length.");
	}
}

//Convert an encoded string into a Message object
void MessageSerialization::decode(const std::string &encoded_msg_, Message &msg) {
	if (encoded_msg_.empty() || encoded_msg_.back() != '\n') {
		throw InvalidMessage("Encoded message must end with a newline character.");
	}
	if (encoded_msg_.size() > Message::MAX_ENCODED_LEN) {
		throw InvalidMessage("Encoded message exceeds maximum length.");
	}

	std::istringstream iss(encoded_msg_); //load message string into a ss
	std::string message_type_str;
	iss >> message_type_str;
	MessageType message_type = string_to_message_type(message_type_str);
	if (message_type == MessageType::NONE) { //check the message type first
		throw InvalidMessage("Invalid message type.");
	}
	msg = Message(message_type);

	// Parse the arguments for the specific message type
	parse_message_arguments(iss, msg, message_type);

	if (!msg.is_valid()) {
		throw InvalidMessage("Decoded message is not valid.");
	}
}