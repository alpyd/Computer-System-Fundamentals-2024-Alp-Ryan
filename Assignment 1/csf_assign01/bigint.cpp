#include <cassert>
#include "bigint.h"


BigInt::BigInt()
{
}

BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative)
{
  this->magnitude = vals;
  this->negative = negative;
}

BigInt::BigInt(uint64_t val, bool negative)
{
  this->magnitude.push_back(val);
  this->negative = negative;
}

BigInt::BigInt(const BigInt &other)
{
  this->magnitude = other.magnitude;
  this->negative = other.negative;
}

BigInt::~BigInt()
{
}

BigInt &BigInt::operator=(const BigInt &rhs)
{
  if (this != &rhs){
    this->magnitude = rhs.magnitude;
    this->negative = rhs.negative;
  }

  return *this;
}

bool BigInt::is_negative() const
{
  return this->negative;
}

uint64_t BigInt::get_bits(unsigned index) const
{
  if(index < magnitude.size()) { //valid index
    return magnitude[index];
  }
  else { //index out of bounds
    return 0;
  }
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return this->magnitude;
}

std::string BigInt::to_hex() const
{
  
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
    //res_magnitude.reserve(max_size);

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
    std::vector<uint64_t> res_magnitude;
    
    bool lhs_greater = compare_magnitudes(lhs, rhs) >= 0; //1 if lhs equal/greater, 0 if rhs greater 

    const auto &larger_magnitude =  lhs_greater ? lhs_magnitude : rhs_magnitude;
    const auto &smaller_magnitude =  lhs_greater ? rhs_magnitude : lhs_magnitude;

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

BigInt BigInt::operator+(const BigInt &rhs) const 
{
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

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b

    //To perform a - b, do a + -b
    return add_magnitudes(*this, -rhs);

}

BigInt BigInt::operator-() const
{
  BigInt opposite_BigInt = *this;
  if(opposite_BigInt.negative == true){
    opposite_BigInt.negative = false;
  } else {
    opposite_BigInt.negative = true;
  }
  return opposite_BigInt;
}

bool BigInt::is_bit_set(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator<<(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
}

int BigInt::compare(const BigInt &rhs) const
{
  // TODO: implement
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}


