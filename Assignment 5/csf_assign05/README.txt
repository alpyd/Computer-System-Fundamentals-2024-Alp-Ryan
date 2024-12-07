ASSIGNMENT 5, SECTION 2

CONTRIBUTIONS

Ryan wrote the foundation for the server and client connection class, as well as introducing exception handling.
Alp expanded on this foundation to handle explicit transactions, as well as introducing FailedTransaction handling, in addition to writing the report.
Both partners helped debugged the code.

REPORT

Synchronization is necessary for all data structures in which multiple threads can manipulate the same object, as this process ensures that information within the data structure can be safely modified without causing unexpected errors.
For the current assignment, the private map within the Server object called "tables" that maps a Table object's string name to a pointer of the specific Table is the only data structure that needs to be synchronized because multiple client threads are using the collection.

The tables data structure was synchronized by utilizing mutexes in critical sections where multiple threads could access the collection at the same time to lock the map for exclusive access to one thread, leading to the other threads to wait until the process was done.
Once the thread utilized the collection for the desired action, such as modifying the data collection using GET or SET in the ClientConnection Oobect or calling find_table and create_table in the Server object, the data collection is then unlocked to allow for the next thread in line to wake up and acquire the data collection.

The server was tested to be free of race conditions and deadlocks in the code's critical sections by ensuring runtime tests between different clients and the server did not stall indefinitely, an indicator of deadlock, as well as analyzing the code's critical sections to ensure the correct order of locking and unlocking of different mutexes.
This way, common causes of deadlock, such as Thread 1 acquiring a lock and waiting to acquire Thread 2's lock and Thread 2 acquiring a lock and waiting to acquire Thread 1's lock, were avoided.
Additionally, by releasing the lock as soon as the Thread is completed with the task, deadlocks arising from indefinite waiting were also avoided.


ASSIGNMENT 5, SECTION 1

CONTRIBUTIONS

Ryan wrote the implementation for message.h, message.cpp, message_serialization.h, message_serialization.cpp, and get_value.cpp.
Alp wrote the implementation for value_stack.h, value_stack.cpp, table.h, table.cpp, set_value.cpp, and incr_value.cpp.

The two partners also debugged each other's code.

One interesting implementation was utilizing the C++ data structures to better represent the value_stack and table objects.
As opposed to coding out the methods for the underlying data structures, utilizing the imported data structures allowed for a simplified and efficient implementation of the methods.

Additionally, for the table class, it was decided that has_tentative_changes would not be true if the set value for a key was equal to the same value for the committed map.
This approach ensured that the boolean represented the state of the map correctly.
