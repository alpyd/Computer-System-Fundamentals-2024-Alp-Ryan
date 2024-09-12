#include <cassert>
#include "bigint.h"
#include <sstream> // For std::stringstream
#include <iomanip> // For std::setfill, std::setw
#include <string>  // For std::string
#include <iostream>
#include <algorithm>

//Constructor for BigInt with no parameters. Makes an empty uint_64 vector to symbolize 0 and sets the negativity to false.
BigInt::BigInt() {
  this->magnitude = std::vector<u_int64_t>{};
  this->negative = false;
}

//Constructor for BigInt with a uint64_t initializer list and predetermined negativity boolean.
BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative) {
  this->magnitude = vals;
  this->negative = negative;
}

//Constructor for BigInt with a single uint64_t value and predetermined negativity boolean.
BigInt::BigInt(uint64_t val, bool negative) {
  this->magnitude.push_back(val);
  this->negative = negative;
}

//Constructor for BigInt object with another BigInt object passed in as a parameter. It copies the negativity boolean and magnitude vector.
BigInt::BigInt(const BigInt &other) {
  this->magnitude = other.magnitude;
  this->negative = other.negative;
}

//Destructor for BigInt. Destruction of vector is already handled.
BigInt::~BigInt() {
}

//Definition of the = operator to create a new BigInt. Outputs a BigInt with same magnitude vector and negativity boolean of the rhs.
BigInt &BigInt::operator=(const BigInt &rhs) {
  //Checks to make sure it is not setting something equal to itself
  if (this != &rhs){
    this->magnitude = rhs.magnitude;
    this->negative = rhs.negative;
  }

  return *this;
}

//Returns whether or not the BigInt value is negative.
bool BigInt::is_negative() const {
  return this->negative;
}

void BigInt::setNegative(bool is_negative) {
  this->negative = is_negative;
}

//Returns the uint64_t value of a certain number at the given index of the magnitude vector. Returns 0 if it is out of the index.
uint64_t BigInt::get_bits(unsigned index) const {
  if(index < magnitude.size()) { //valid index
    return magnitude[index];
  }
  else { //index out of bounds
    return 0;
  }
}

//Returns the magnitude vector of the BigInt.
const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return this->magnitude;
}


//Converts the BigInt into a hexadecimal number by incorporating a negative sign and the hexadecimal representation of the magnitude vector
std::string BigInt::to_hex() const {
  
  std::stringstream ss;
  if(negative) { //add negative sign for negative values
    ss << "-";
  }
  bool leading_zero = true;

  //Start from most significant uint64_t in magnitude
 for(auto it = magnitude.rbegin(); it != magnitude.rend(); ++it) {
  
  if(*it == 0 && leading_zero) { //skip leading zeroes
    continue;
  }

  if(leading_zero) { //once non-zero encountered
    ss << std::hex << *it; 
    leading_zero = false; //stop skipping zeroes
  } else { //ensure hex formatting & leading zero padding if necessary
    ss << std::setfill('0') << std::setw(16) << *it;
  }
  
 }

  if(leading_zero) { //If all zeroes or empty magnitude vector
    return "0";
  }

  return ss.str();
}

//This helper method sets a new magnitude vector for the current BigInt
void BigInt::setMagnitude(const std::vector<uint64_t>& newMagnitude) {
  magnitude = newMagnitude;
}

//Returns 1 if LHS is larger, -1 if RHS is larger, 0 if equal
static int compare_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  BigInt editable_rhs = BigInt(rhs);
  BigInt editable_lhs = BigInt(lhs);
  editable_lhs.trim_leading_zeroes();
  editable_rhs.trim_leading_zeroes();

  const auto &lhs_magnitude = editable_lhs.get_bit_vector();
  const auto &rhs_magnitude = editable_rhs.get_bit_vector();

  //Compare based on number of significant digits (size of vector)
  if (lhs_magnitude.size() != rhs_magnitude.size()) {
    return lhs_magnitude.size() > rhs_magnitude.size() ? 1 : -1;
  }
    
  //Compare element by element starting from most significant bit
  for (int i = lhs_magnitude.size() - 1; i >= 0; --i) { 
    if (lhs_magnitude[i] != rhs_magnitude[i]) {
      return lhs_magnitude[i] > rhs_magnitude[i] ? 1 : -1;
    }
  }

  return 0; //equal magnitude
}

//This helper method adds the magnitude vectors of the left hand and right hand BigInts
static BigInt add_magnitudes(const BigInt &lhs, const BigInt &rhs) {  
  BigInt result;
  const auto& lhs_magnitude = lhs.get_bit_vector();
  const auto& rhs_magnitude = rhs.get_bit_vector();

  size_t max_size = std::max(lhs_magnitude.size(), rhs_magnitude.size()); //max number of digits to be processed
  std::vector<uint64_t> res_magnitude;
  res_magnitude.reserve(max_size);

  uint64_t carry = 0; //used for grade school algo
  for (size_t i = 0; i < max_size; ++i) {
    //access i^th element, if past the size assume it is a 0
    uint64_t lhs_val = i < lhs_magnitude.size() ? lhs_magnitude[i] : 0;
    uint64_t rhs_val = i < rhs_magnitude.size() ? rhs_magnitude[i] : 0;

    uint64_t sum = lhs_val + rhs_val + carry;
    
    //check if the addition resulted in an overflow
    carry = (sum < lhs_val) ? 1 : 0;

    //store result of current digit addition
    res_magnitude.push_back(sum);
  }

  if (carry) { //check for any carry, adds if needed
    res_magnitude.push_back(carry);
  }

  result.setMagnitude(res_magnitude); //assign result vector to BigInt
  return result;
}

//This helper method carries out the subtraction of magnitudes of two different BigInts
static BigInt subtract_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  BigInt result;
  const auto &lhs_magnitude = lhs.get_bit_vector();
  const auto &rhs_magnitude = rhs.get_bit_vector();
  
  bool lhs_greater = compare_magnitudes(lhs, rhs) >= 0; //1 if lhs equal/greater, 0 if rhs greater 
  const auto &larger_mag =  lhs_greater ? lhs_magnitude : rhs_magnitude;
  const auto &smaller_mag =  lhs_greater ? rhs_magnitude : lhs_magnitude;

  std::vector<uint64_t> res_magnitude;

  uint64_t carry = 0;
  for (size_t i = 0; i < larger_mag.size(); ++i) {
    //access i^th element, if past the size assume it is a 0
    uint64_t first_digit = larger_mag[i];
    uint64_t second_digit = i < rhs_magnitude.size() ? smaller_mag[i] : 0;

    uint64_t diff = first_digit - second_digit - carry; //do subtraction
    carry = (first_digit < (second_digit + carry)) ? 1 : 0; //determine if carry needed

    res_magnitude.push_back(diff); //store difference of operands
  }

  result.setMagnitude(res_magnitude);

  //Remove zeroes at end of vector
  result.trim_leading_zeroes();

  if (!lhs_greater) {
    result = -result; // adjust sign if first operand had smaller magnitude
  }

  return result;
}

// This method carries out the addition of the current BigInt with the right hand side BigInt to return the sum of the BigInt
BigInt BigInt::operator+(const BigInt &rhs) const {
  BigInt result;
   if (this->negative == rhs.negative) { //Same sign, just add their magnitudes
        result = add_magnitudes(*this, rhs);
        result.negative = this->negative;
    } else { // If they have opposite signs, subtract magnitudes, make sure larger - smaller
      if (compare_magnitudes(*this, rhs) > 0) { // *this has larger magnitude
          result = subtract_magnitudes(*this, rhs);
          result.negative = this->negative; // result takes the sign of the larger magnitude

      } else if(compare_magnitudes(*this, rhs) < 0) { //rhs has larger magnitude
          result = subtract_magnitudes(rhs, *this);
          result.negative = rhs.negative;
          
      } else { //equal magnitudes
        return BigInt(); //the sum is the BigInt 0
      }
    }
    return result;
}

//This method carries out the subtraction of the current BigInt with the right hand side BigInt to return the difference of the two.
BigInt BigInt::operator-(const BigInt &rhs) const {
  
    //To perform a - b, do a + -b
    return *this + -rhs;

}

//Returns true if the BigInt corresponds to value 0, false otherwise
bool BigInt::is_zero() const {
  bool zeroes_only = true;

  if(this->magnitude.size() == 0) { //no magnitude means 0
    return true;
  }
  for(size_t i = 0; i < this->magnitude.size(); ++i) {
    if(this->magnitude[i] != 0) {
      zeroes_only = false; //if we find a nonzero element, cannot be 0
    }
  }
  return zeroes_only;
}

//This method returns the negative version of the BigInt by flipping the negative value.
BigInt BigInt::operator-() const {
  if(this->is_zero()) { //cannot negate 0
    return *this;
  } else {
    BigInt opposite_BigInt = *this;
    opposite_BigInt.negative = !(this->negative);
    return opposite_BigInt;
  }
}

//This function checks whether the bit at the provided index is a 1 or 0 and returns true if it is a 1 and false if it is a 0
bool BigInt::is_bit_set(unsigned n) const {
  unsigned vector_index = n / 64; //index of uint64_t's with vector (0,1,...)
  unsigned bit_index = n % 64; //bit index within the uint64_t (64 bits)

  if(vector_index >= this->magnitude.size()) { //bit is not in set
    return false;
  }

  uint64_t value = this->magnitude[vector_index];
  uint64_t mask = uint64_t(1) << bit_index; //set all bits to 0 besides n^th bit
  return (value & mask) != 0; //check n^th bit position in uint64_t value

  //bitwise AND will return 0 if the bit was 0, and nonzero if bit was 1

}

//This function left shifts the magnitude vector by the specified number of iterations
BigInt BigInt::operator<<(unsigned n) const {
  if(this->negative) {
    throw std::invalid_argument("Left shift not allowed for negative values");
  }

  if(n == 0) { //no shift performed
    return *this;
  }

  BigInt result;

  //How many uint64_t's to shift left
  int shift_index = n / 64;

  //How many bits to shift within a uint64_t 
  int shift_bits = n % 64; 

  //Add uint64 0's as a result of left shift
  result.magnitude.resize(this->magnitude.size() + shift_index, 0);

  // Shift the magnitude blocks
    uint64_t carry = 0; // Store bits that spill over from lower part
    for (size_t i = 0; i < this->magnitude.size(); ++i) {
        uint64_t current = this->magnitude[i];

        // Shift the current block left and add carry from the previous block
        result.magnitude[i + shift_index] = (current << shift_bits) | carry;

        // Calculate new carry (spillover bits that shift out of the current block)
        carry = (shift_bits > 0) ? (current >> (64 - shift_bits)) : 0;
    }

    // If there is still a carry left, append it as a new block
    if (carry > 0) {
        result.magnitude.push_back(carry);
    }

  return result;
}

//This function carries out the multiplication between the left hand side and the right hand side and returns their product as a BigInt
BigInt BigInt::operator*(const BigInt &rhs) const {
  
  BigInt result;
  if(this->is_zero() || rhs.is_zero()) { //multiplying 0 yields 0
    return BigInt();
  }
  
  //Use absolute values of operands to be able to left shift
  BigInt lhs_abs = *this; 
  BigInt rhs_abs = rhs;
  if (lhs_abs.negative) lhs_abs.negative = false;
  if (rhs_abs.negative) rhs_abs.negative = false;

  BigInt temp = lhs_abs;
  for (unsigned i = 0; i < rhs_abs.magnitude.size() * 64; ++i) {
        if (rhs_abs.is_bit_set(i)) {
            // If the i^th bit is 1, add *this shifted left by i bits to multiply
            result = result + (temp << i);
        }
    }

  //if both operands have same sign, result is positive
  //if mixed signs will be negative
  result.negative = (this->negative != rhs.negative); 
  return result;

}

//This function carries out a binary search to divide the lhs by the rhs
static BigInt binary_search(const BigInt &lhs, const BigInt &rhs) {

    BigInt dividend = lhs;
    dividend.setNegative(false);

    BigInt divisor = rhs;
    divisor.setNegative(false);

    // Binary search for the quotient
    BigInt lower_bound(0);
    BigInt upper_bound = dividend + BigInt(1);

    BigInt quotient;

    while (lower_bound + BigInt(1) < upper_bound) {
      BigInt mid = (lower_bound + upper_bound).div_by_2();  // divide by 2
      BigInt product = mid * divisor; //see how close to dividend

      if (product == dividend) {
          return mid;
      } else if (product < dividend) { //increase lower bound
          lower_bound = mid;
      } else { //decrease upper bound
          upper_bound = mid;
      }
    }

  return lower_bound;

}

//This function divides the Implicit BigInt (this*) by 2
BigInt BigInt::div_by_2() const {
    BigInt result;
    uint64_t carry = 0;

    // Iterate over the magnitude from the most significant to least significant block
    for (size_t i = this->magnitude.size(); i > 0; --i) {
        // Get current block
        uint64_t current = this->magnitude[i - 1];

        // Shift right and incorporate any carry from the previous block
        result.magnitude.insert(result.magnitude.begin(), (current >> 1) | carry);

        // Prepare carry for the next iteration, which is the least significant bit
        carry = (current & 1) ? (1ULL << 63) : 0;  // If LSB is 1, it carries over as MSB
    }

    // Keep same sign as *this
    result.negative = this->negative;
    
    // Trim leading zeroes if necessary
    result.trim_leading_zeroes();

    return result;
}

//This method removes the leading zeros for an array that do not influence the magnitude
void BigInt::trim_leading_zeroes() {
    // Keep removing the most significant uint64_t 0 blocks
    while (!magnitude.empty() && magnitude.back() == 0) {
        magnitude.pop_back();
    }

    // If all blocks are zero, reset sign to non-negative
    if (magnitude.empty()) {
        negative = false;
    }
}

//This method divides the BigInt (*this) by the rhs
BigInt BigInt::operator/(const BigInt &rhs) const {
  if(rhs.is_zero()) {
    throw std::invalid_argument("Cannot divide by zero");
  }

  //dividend 0 leads to quotient 0
  if(this->is_zero()) { 
    return BigInt();
  }

  BigInt result = binary_search(*this, rhs);
  
  if(!result.is_zero()) { //adjust sign if nonzero
    result.negative = (this->negative != rhs.negative); //same sign = positive, different = negative
  }
  return result;
}

// This method compares the current BigInt with the right hand side BigInt
// and returns 1 if the current is larger, 0 if they are equal, and -1 if the rhs is larger.
int BigInt::compare(const BigInt &rhs) const {
  if(this->negative && !rhs.negative) { //LHS < RHS
    return -1;
  }
  if(rhs.negative && !this->negative) { //LHS > RHS
    return 1;
  }

  //BigInts at this point must have same sign, now compare magnitude
  //Returns 1 if LHS is larger, -1 if RHS is larger, 0 if equal
  int compare_mag = compare_magnitudes(*this, rhs);

  if(this->negative && rhs.negative) { //both values negative,
    return -compare_mag; //larger negative magnitude is actually smaller
  } else { //both values positive
    return compare_mag;
  }
}

//has_non_zero() is a helper function that checks if there are any non-zero indices in all of the magnitude vectors
bool BigInt::has_non_zero() const {
  bool has_nonzero = false;
  for(size_t i = 0; i < this->magnitude.size(); i++){
    if(magnitude[i] != 0){
      has_nonzero = true;
    } 
  } 
  return has_nonzero;
}

//To Decimal Function That Converts BigInt magnitude vector to a String
std::string BigInt::to_dec() const {
  //Return 0 if the BigInt equals 0
  if(magnitude.size() == 0 || !has_non_zero()){
    return "0";
  }

  //Create string placeholders and BigInt representations of number to be able to execute program
  std::string dec = ""; 
  std::string dec_rev = "";
  BigInt ten = BigInt(10, false);
  BigInt copy = BigInt(*this);
  BigInt zero = BigInt(0, false);

  //Edge Case: Remove any Zeros in the beginning of the magntidue
  copy.trim_leading_zeroes();

  //Take the mod 10 of the number to get the digit and then divide by 10 continuously until the BigInt equals zero.
  while(copy.compare(zero) != 0){
    BigInt division = copy/ten;
    BigInt mod = copy - ten * division;
    copy = division;
    char c = mod.get_bits(0) + '0';
    dec_rev.push_back(c);
  }

  // Reverse the string to ensure that the largest digit is first
  for(int i = dec_rev.size() - 1; i >= 0; i--){
    dec.push_back(dec_rev[i]);
  }

  // Add a negative sign if the number string is negative 
  if(this->negative){
    dec.insert(dec.begin(), '-');
  }
  return dec;
}