CONTRIBUTIONS

Ryan wrote the implementation for message.h, message.cpp, message_serialization.h, message_serialization.cpp, and get_value.cpp.
Alp wrote the implementation for value_stack.h, value_stack.cpp, table.h, table.cpp, set_value.cpp, and incr_value.cpp.

The two partners also debugged each other's code.

One interesting implementation was utilizing the C++ data structures to better represent the value_stack and table objects.
As opposed to coding out the methods for the underlying data structures, utilizing the imported data structures allowed for a simplified and efficient implementation of the methods.

Additionally, for the table class, it was decided that has_tentative_changes would not be true if the set value for a key was equal to the same value for the committed map.
This approach ensured that the boolean represented the state of the map correctly.