#include <cassert>
#include "bigint.h"
#include <sstream> // For std::stringstream
#include <iomanip> // For std::setfill, std::setw
#include <string>  // For std::string

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

void BigInt::setMagnitude(const std::vector<uint64_t>& newMagnitude) {
  magnitude = newMagnitude;
}

//Returns 1 if LHS is larger, -1 if RHS is larger, 0 if equal
static int compare_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  const auto &lhs_magnitude = lhs.get_bit_vector();
  const auto &rhs_magnitude = rhs.get_bit_vector();

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


static BigInt subtract_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  BigInt result;
  const auto &lhs_magnitude = lhs.get_bit_vector();
  const auto &rhs_magnitude = rhs.get_bit_vector();
  
  bool lhs_greater = compare_magnitudes(lhs, rhs) >= 0; //1 if lhs equal/greater, 0 if rhs greater 
  const auto &larger_magnitude =  lhs_greater ? lhs_magnitude : rhs_magnitude;
  const auto &smaller_magnitude =  lhs_greater ? rhs_magnitude : lhs_magnitude;

  std::vector<uint64_t> res_magnitude;

  uint64_t carry = 0;
  for (size_t i = 0; i < larger_magnitude.size(); ++i) {
    //access i^th element, if past the size assume it is a 0
    uint64_t first_digit = larger_magnitude[i];
    uint64_t second_digit = i < rhs_magnitude.size() ? smaller_magnitude[i] : 0;

    uint64_t diff = first_digit - second_digit - carry; //do subtraction
    carry = (first_digit < (second_digit + carry)) ? 1 : 0;

    res_magnitude.push_back(diff);
  }

  //Remove zeroes at end of vector
  while (res_magnitude.size() > 1 && res_magnitude.back() == 0) {
    res_magnitude.pop_back();
  }

  result.setMagnitude(res_magnitude);

  if (!lhs_greater) {
    result = -result; // adjust sign if first operand had smaller magnitude
  }

  return result;
}

//BigInt div_by_2() const;

BigInt BigInt::operator+(const BigInt &rhs) const {
  BigInt result;
   if (this->negative == rhs.negative) { ////Same sign, just add their magnitudes
        result = add_magnitudes(*this, rhs);
        result.negative = this->negative;
    } else { // If they have opposite signs, convert to subtraction, make sure larger - smaller
        if (compare_magnitudes(*this, rhs) >= 0) { // *this has larger magnitude
            result = subtract_magnitudes(*this, rhs);
            result.negative = this->negative; // result takes the sign of the larger magnitude

        } else { //rhs has larger magnitude
            BigInt result = subtract_magnitudes(rhs, *this);
            result.negative = rhs.negative;
            
        }

    }
    return result;
}

BigInt BigInt::operator-(const BigInt &rhs) const {
  // TODO: implement
  // Hint: a - b could be computed as a + -b

    //To perform a - b, do a + -b
    return add_magnitudes(*this, -rhs);

}

//Returns true if the BigInt corresponds to value 0, false otherwise
bool BigInt::is_zero() const {
  bool zeroes_only = true;

  if(this->magnitude.size() == 0) { //no magnitude means 0
    return false;
  }
  for(size_t i = 0; i < this->magnitude.size(); ++i) {
    if(this->magnitude[i] != 0) {
      zeroes_only = false; //if we find a nonzero element, cannot be 0
    }
  }
  return zeroes_only;
}

BigInt BigInt::operator-() const {
  if(this->magnitude.size() == 0) {
    return *this;
  } else {
    BigInt opposite_BigInt = *this;
    opposite_BigInt.negative = !(this->negative);
    return opposite_BigInt;
  }

}

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

BigInt BigInt::operator<<(unsigned n) const {
  if(this->negative) {
    throw std::invalid_argument("Left shift not allowed for negative values");
  }

  BigInt result(*this);
  
  //TODO: implement left shift by n bits



  return result;

}

BigInt BigInt::operator*(const BigInt &rhs) const {
  // TODO: implement
}

BigInt BigInt::operator/(const BigInt &rhs) const {
  // TODO: implement
}

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

std::string BigInt::to_dec() const {
  std::string hex = this->to_hex();
  std::istringstream stream(hex);
  std::vector<int> decimal;
  std::string dec;

  int remainder = 0;
  char hex_char = 'z';

  while (stream >> hex_char) {
    int digit = -1;
    if (hex_char >= '0' && hex_char <= '9') {
      digit = hex_char - '0';
    } else if (hex_char >= 'a' && hex_char <= 'f') {
      digit = 10 + (hex_char - 'a');
    } 
      int hex_value = 16 * digit + remainder;
      decimal.push_back(hex_value/10);
      remainder = (hex_value % 10) * 16;
  }
  int original_decimal_length = decimal.size();
  int added_indices = 0;
  for (size_t i = original_decimal_length - 1; i >= 0; i++) {
    int digit = decimal[i + added_indices];
    int test_digit = digit;
    int order_of_magnitude = 0;
      while(test_digit >= 10){
        test_digit /= 10;
        order_of_magnitude++;
      }
    for (int j = 0; j < order_of_magnitude; j++){
      int added_digit = digit % 10;
      digit = (digit - added_digit) / 10;
      if(j == 0){
        decimal[i + added_indices] = added_digit;
      } else {
        if((i + added_indices - j) < 0){
          decimal.insert(decimal.begin(), added_digit);
          added_indices++;
        } else {
          decimal[i + added_indices - j] += added_digit;
        }
      }
    }
  }
  for (size_t i = 0; i < decimal.size(); i++){
    char dec_char = decimal[i] + '0';
    dec.push_back(dec_char);
  }

  return dec;
}


