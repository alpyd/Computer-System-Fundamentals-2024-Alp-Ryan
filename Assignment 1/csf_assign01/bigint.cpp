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

bool is_zero() const;
static BigInt add_magnitudes(const BigInt &lhs, const BigInt &rhs);


static BigInt subtract_magnitudes(const BigInt &lhs, const BigInt &rhs) {
    
    BigInt result;
    result.magnitude.clear();

    const size_t n = lhs.magnitude.size();
    uint64_t borrow = 0;

    for (size_t i = 0; i < n; ++i) {
        uint64_t lhs_val = lhs.magnitude[i];
        uint64_t rhs_val = i < rhs.magnitude.size() ? rhs.magnitude[i] : 0;

        uint64_t diff = lhs_val - rhs_val - borrow;
        borrow = lhs_val < rhs_val + borrow;  // When subtracting, check for underflow
        result.magnitude.push_back(diff);
    }

    // Remove leading zeros
    while (result.magnitude.size() > 1 && result.magnitude.back() == 0) {
        result.magnitude.pop_back();
    }

    return result;
}

//Returns 1 if LHS is larger, -1 if RHS is larger, 0 if equal
static int compare_magnitudes(const BigInt &lhs, const BigInt &rhs) {

  if (lhs->magnitude.size() != rhs.magnitude.size()) {
        return lhs->magnitude.size() > rhs.magnitude.size() ? 1 : -1; //compare by size of vector
    }
    
    for (int i = lhs->magnitude.size() - 1; i >= 0; --i) { //start from most significant bit
        if (lhs->magnitude[i] != rhs.magnitude[i]) {
            return lhs->magnitude[i] > rhs.magnitude[i] ? 1 : -1;
        }
    }

    return 0; //equal magnitude
}

BigInt div_by_2() const;

BigInt BigInt::operator+(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b

  if (this->negative == rhs.negative) {
        BigInt result;

        if (this->compare(rhs) >= 0) {
            result = BigInt::subtract_magnitudes(*this, rhs);
            result.negative = this->negative;
        } else {
            result = BigInt::subtract_magnitudes(rhs, *this);
            result.negative = !rhs.negative;
        }

        return result;
    } else {
        return *this + (-rhs);  // Convert subtraction to addition if signs differ
    }


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

std::string BigInt::to_hex() const
{
  const char hex_characters[] = "0123456789abcdef";
  std::string hex = "";
  if(magnitude.size() == 0){
    return "0";
  }
  for (size_t i = 0; i < magnitude.size(); i++) {
    uint64_t remaining_value = magnitude[i] + 64*magnitude[i+1];
    for(int i = 0; i < 16; i++) {
      char single_digit = hex_characters[remaining_value % 16];
      remaining_value = (remaining_value - remaining_value % 16)/16;
      hex = single_digit + hex;
    }
  }
  while(hex.at(0) == 0) {
    hex.erase(0, 1);  
  }
  return hex;
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}


