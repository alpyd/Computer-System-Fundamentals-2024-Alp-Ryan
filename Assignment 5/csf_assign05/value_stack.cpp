#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack()
{
}

ValueStack::~ValueStack()
{
}

bool ValueStack::is_empty() const
{
  return value_stack.empty();
}

void ValueStack::push( const std::string &value )
{
  value_stack.push(value);
}

std::string ValueStack::get_top() const
{
  if(value_stack.empty()) {
    throw OperationException("Cannot get the top of an empty ValueStack");
  }
  return value_stack.top();
}

void ValueStack::pop()
{
  if(value_stack.empty()) {
    throw OperationException("Cannot pop the top of an empty ValueStack");
  }
  value_stack.pop();
}
