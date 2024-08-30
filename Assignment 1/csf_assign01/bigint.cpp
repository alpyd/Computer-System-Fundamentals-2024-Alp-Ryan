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

BigInt BigInt::operator+(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b
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
    for(int i = 0; i < 16; i++){
      char single_digit = hex_characters[remaining_value % 16];
      remaining_value = (remaining_value - remaining_value % 16)/16;
      hex = single_digit + hex;
    }
  }
  while(hex.at(0) == 0){
    hex.erase(0, 1);  
  }
  return hex;
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}


