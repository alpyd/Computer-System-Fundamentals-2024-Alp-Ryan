#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack() {
}

ValueStack::~ValueStack() {
}

// Return true if the ValueStack Object is empty
bool ValueStack::is_empty() const {
  return value_stack.empty();
}

// Push a provided string value into the ValueStack
void ValueStack::push( const std::string &value ) {
  value_stack.push(value);
}

// Return the top string on the ValueStack
std::string ValueStack::get_top() const {
  // Throw an Exception if the ValueStack is empty; No top to get
  if(value_stack.empty()) {
    throw OperationException("Cannot get the top of an empty ValueStack");
  }
  // Return the top string of the ValueStack
  return value_stack.top();
}

// Pop the top of the ValueStack
void ValueStack::pop() {
  // Throw an Exception if the ValueStack is empty; Nothing to pop()
  if(value_stack.empty()) {
    throw OperationException("Cannot pop the top of an empty ValueStack");
  }
  // Pop the top value in the ValueStack
  value_stack.pop();
}
