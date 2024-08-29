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
  // TODO: implement
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
  // TODO: implement
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
  // TODO: implement
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}

