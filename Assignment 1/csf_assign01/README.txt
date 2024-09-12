Alp Demirtas (ademirt1)  
Ryan Amer (ramer2)

Assignment 1 Contributions:
Both partners contributed equally to the assignment.
Alp wrote the constructor for the BigInt class and wrote the first drafts of the is_negative, operator=, get_bits, get_bit_vector, to_hex, and to_dec methods. 
Ryan  wrote the set_magnitude, add_magnitudes, subtract_magnitudes, operator+, operator-, is_bit_set, operator<<, operator/, operator*, and compare methods.
Both partners edited each other's methods, as well as developed helper methods for their respective implementations.

Alp wrote the following unit tests: hw1_constructors_equals_tests, hw1_get_bits_get_bit_vector_tests, hw_1_unary_is_negative_tests, hw_1_to_hex_tests, hw1_is_bit_set_tests, hw1_compare_tests, hw1_left_shift_tests, and hw1_to_dec_tests.
Ryan wrote the following unit tests: hw1_addition_tests, hw1_subtract_tests, hw1_multiply_tests, and hw1_divide_tests.

 

Interesting Implementations:
An interesting implementation is that an intialization of a BigInt object without any elements in the magnitude array and the negative boolean being false. While there were many ways to demonstrate 0, we chose this approach to easily distinguish a 0 value with the length being equal to 0.
Additionally, the to_dec implementation was interesting because by declaring 10 as a BigInt instead of one of the C primitive types, it utilized the previous BigInt methods such as *, /, and -.