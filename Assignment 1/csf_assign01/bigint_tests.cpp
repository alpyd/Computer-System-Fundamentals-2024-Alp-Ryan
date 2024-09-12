#include <stdexcept>
#include <sstream>
#include <iostream>
#include "bigint.h"
#include "tctest.h"

struct TestObjs {
  BigInt zero;
  BigInt one;
  BigInt two;
  BigInt u64_max;
  BigInt two_pow_64;
  BigInt negative_two_pow_64;
  BigInt three;
  BigInt negative_nine;
  BigInt negative_three;
  BigInt nine;
  BigInt two_pow_64_plus_one;
  BigInt two_pow_64_plus_hex;
  BigInt multiple_zeros;
  BigInt really_big_number;
  BigInt two_pow_65;
  BigInt negative_one;
  

  TestObjs();
};

// prototypes for setup and cleanup functions
TestObjs *setup();
void cleanup(TestObjs *objs);

// Verify that a BigInt contains appropriate data by checking the
// contents of its internal vector of uint64_t values.
// This allows us to validate the contents of a BigInt object
// without needing to rely on member functions other than get_bit_vector().
// Throws std::runtime_error if the actual values don't exactly match
// the expected values.
void check_contents(const BigInt &bigint, std::initializer_list<uint64_t> expected_vals);

// prototypes of test functions
void test_default_ctor(TestObjs *objs);
void test_u64_ctor(TestObjs *objs);
void test_initlist_ctor(TestObjs *objs);
void test_copy_ctor(TestObjs *objs);
void test_get_bits(TestObjs *objs);
void test_add_1(TestObjs *objs);
void test_add_2(TestObjs *objs);
void test_add_3(TestObjs *objs);
void test_add_4(TestObjs *objs);
void test_sub_1(TestObjs *objs);
void test_sub_2(TestObjs *objs);
void test_sub_3(TestObjs *objs);
void test_sub_4(TestObjs *objs);
void test_is_bit_set_1(TestObjs *objs);
void test_is_bit_set_2(TestObjs *objs);
void test_lshift_1(TestObjs *objs);
void test_lshift_2(TestObjs *objs);
void test_mul_1(TestObjs *objs);
void test_mul_2(TestObjs *objs);
void test_compare_1(TestObjs *objs);
void test_compare_2(TestObjs *objs);
void test_div_1(TestObjs *objs);
void test_div_2(TestObjs *objs);
void test_to_hex_1(TestObjs *objs);
void test_to_hex_2(TestObjs *objs);
void test_to_dec_1(TestObjs *objs);
void test_to_dec_2(TestObjs *objs);
void hw1_constructors_equals_tests(TestObjs *objs);
void hw1_get_bits_get_bit_vector_tests(TestObjs *objs);
void hw_1_unary_is_negative_tests(TestObjs *objs);
void hw_1_to_hex_tests(TestObjs *objs);
void hw1_addition_tests(TestObjs *objs);
void hw1_subtract_tests(TestObjs *objs);
void hw1_multiply_tests(TestObjs *objs);
void hw1_divide_tests(TestObjs *objs);
void hw1_is_bit_set_tests(TestObjs *objs);
void hw1_compare_tests(TestObjs *objs);
void hw1_left_shift_tests(TestObjs *objs);
void hw1_to_dec_tests(TestObjs *objs);


int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_default_ctor);
  TEST(test_u64_ctor);
  TEST(test_initlist_ctor);
  TEST(test_copy_ctor);
  TEST(test_get_bits);
  TEST(test_add_1);
  TEST(test_add_2);
  TEST(test_add_3);
  TEST(test_add_4);
  TEST(test_sub_1);
  TEST(test_sub_2);
  TEST(test_sub_3);
  TEST(test_sub_4);
  TEST(test_is_bit_set_1);
  TEST(test_is_bit_set_2);
  TEST(test_lshift_1);
  TEST(test_lshift_2);
  TEST(test_mul_1);
  TEST(test_mul_2);
  TEST(test_compare_1);
  TEST(test_compare_2);
  TEST(test_div_1);
  TEST(test_div_2);
  TEST(test_to_hex_1);
  TEST(test_to_hex_2);
  TEST(test_to_dec_1);
  TEST(test_to_dec_2);
  TEST(hw1_constructors_equals_tests);
  TEST(hw1_get_bits_get_bit_vector_tests);
  TEST(hw_1_unary_is_negative_tests);
  TEST(hw_1_to_hex_tests);
  TEST(hw1_addition_tests);
  TEST(hw1_subtract_tests);
  TEST(hw1_multiply_tests);
  TEST(hw1_divide_tests);
  TEST(hw1_is_bit_set_tests);
  TEST(hw1_compare_tests);
  TEST(hw1_left_shift_tests);
  TEST(hw1_to_dec_tests);


  TEST_FINI();
}


// All initialization of test fixture objects should be done
// in the TestObjs constructor.
TestObjs::TestObjs()
  : zero()
  , one(1UL)
  , two(2UL)
  , u64_max(0xFFFFFFFFFFFFFFFFUL)
  , two_pow_64({ 0UL, 1UL })
  , negative_two_pow_64({ 0UL, 1UL }, true)
  , three(3UL)
  , negative_nine(9UL, true)
  , negative_three(3UL, true)
  , nine(9UL)
  , two_pow_64_plus_one({1UL, 1UL})
  , two_pow_64_plus_hex({257UL, 1UL})
  , multiple_zeros({0UL, 0UL, 0UL})
  , really_big_number({4UL, 7UL, 6UL, 9UL, 0UL, 3UL, 1UL, 2UL, 5UL})
  , two_pow_65({0UL, 2UL})
  , negative_one({1}, true)
{
}

TestObjs *setup() {
  return new TestObjs;
}

void cleanup(TestObjs *objs) {
  delete objs;
}

void check_contents(const BigInt &bigint, std::initializer_list<uint64_t> expected_vals) {
  const std::vector<uint64_t> &actual_vals = bigint.get_bit_vector();

  auto i = actual_vals.begin();
  auto j = expected_vals.begin();

  unsigned count = 0;

  while (i != actual_vals.end() && j != expected_vals.end()) {
    if (*i != *j) {
      std::stringstream msg;
      msg << "in word " << count
          << " actual value " << std::hex << *i
          << " does not match expected value " << std::hex << *j;
      throw std::runtime_error(msg.str());
    }
    ++i;
    ++j;
    ++count;
  }

  // at this point, if either the expected values or actual values
  // have additional data, all of the additional values need to be 0

  while (i != actual_vals.end()) {
    if (*i != 0UL) {
      std::stringstream msg;
      msg << "in word " << count
          << " actual value " << std::hex << *i
          << " does not match expected value 0";
      throw std::runtime_error(msg.str());
    }
    ++i;
    ++count;
  }

  while (j != expected_vals.end()) {
    if (*j != 0UL) {
      std::stringstream msg;
      msg << "in word " << count
          << " actual value 0 "
          << " does not match expected value " << std::hex << *j;
      throw std::runtime_error(msg.str());
    }
    ++j;
    ++count;
  }
}

void test_default_ctor(TestObjs *objs) {
  check_contents(objs->zero, { 0UL });
  ASSERT(!objs->zero.is_negative());
}

void test_u64_ctor(TestObjs *objs) {
  check_contents(objs->one, { 1UL });
  ASSERT(!objs->one.is_negative());

  check_contents(objs->two, { 2UL });
  ASSERT(!objs->two.is_negative());

  check_contents(objs->u64_max, { 0xFFFFFFFFFFFFFFFFUL });
  ASSERT(!objs->u64_max.is_negative());

  check_contents(objs->three, { 3UL });
  ASSERT(!objs->three.is_negative());

  check_contents(objs->negative_nine, { 9UL });
  ASSERT(objs->negative_nine.is_negative());

  check_contents(objs->negative_three, { 3UL });
  ASSERT(objs->negative_three.is_negative());

  check_contents(objs->nine, { 9UL });
  ASSERT(!objs->nine.is_negative());
}

void test_initlist_ctor(TestObjs *objs) {
  check_contents(objs->two_pow_64, { 0UL, 1UL });
  ASSERT(!objs->two_pow_64.is_negative());

  check_contents(objs->negative_two_pow_64, { 0UL, 1UL });
  ASSERT(objs->negative_two_pow_64.is_negative());
}

void test_copy_ctor(TestObjs *objs) {
  BigInt copy(objs->two_pow_64);

  check_contents(copy, { 0UL, 1UL });
  ASSERT(!copy.is_negative());
}

void test_get_bits(TestObjs *objs) {
  ASSERT(0UL == objs->zero.get_bits(0));
  ASSERT(0UL == objs->zero.get_bits(1));

  ASSERT(1UL == objs->one.get_bits(0));
  ASSERT(0UL == objs->one.get_bits(1));

  ASSERT(0xFFFFFFFFFFFFFFFFUL == objs->u64_max.get_bits(0));
  ASSERT(0UL == objs->u64_max.get_bits(1));

  ASSERT(0UL == objs->two_pow_64.get_bits(0));
  ASSERT(1UL == objs->two_pow_64.get_bits(1));
}

void test_add_1(TestObjs *objs) {
  // very basic tests for addition

  BigInt result1 = objs->zero + objs->zero;
  check_contents(result1, { 0UL });
  ASSERT(!result1.is_negative());

  BigInt result2 = objs->zero + objs->one;
  check_contents(result2, { 1UL });
  ASSERT(!result2.is_negative());

  BigInt result3 = objs->one + objs->zero;
  check_contents(result3, { 1UL });
  ASSERT(!result3.is_negative());

  BigInt result4 = objs->u64_max + objs->one;
  check_contents(result4, { 0UL, 1UL });
  ASSERT(!result4.is_negative());
}

void test_add_2(TestObjs *) {
  // Harder addition test(s)
  {
    BigInt left({0xc26e6d5f53ab28a0UL, 0xfb12a615f71867UL, 0x1f12c444bbc13UL});
    BigInt right({0x68adaac8bc0c460aUL, 0x2b287106b393UL});
    BigInt result = left + right;
    check_contents(result, {0x2b1c18280fb76eaaUL, 0xfb3dce86fdcbfbUL, 0x1f12c444bbc13UL});
    ASSERT(!result.is_negative());
  }
}

void test_add_3(TestObjs *objs) {
  // Test additions with mixed signs

  BigInt result1 = objs->three + objs->negative_nine;
  check_contents(result1, { 6UL });
  ASSERT(result1.is_negative());

  BigInt result2 = objs->negative_nine + objs->three;
  check_contents(result2, { 6UL });
  ASSERT(result2.is_negative());
}

void test_add_4(TestObjs *) {
  // Harder addition test(s) with negative and nonnegative operands
  {
    BigInt left({0x9f33ca9e0290d102UL, 0xd331662e19562498UL, 0x2347e72885d6e83UL, 0x529bac52c46b047UL, 0x69b3f801ea78f778UL, 0xfdba8845fc024708UL, 0x2UL});
    BigInt right({0x3efacbd8f95c7bbUL, 0x974f58eddfc56dfeUL, 0xa844e13b7c33ebedUL, 0xb4dd001fbdf5f260UL, 0x755c8aee33f48fecUL, 0x60UL}, true);
    BigInt result = left + right;
    check_contents(result, {0x9b441de072fb0947UL, 0x3be20d403990b69aUL, 0x59ef9d370c298296UL, 0x504cbaa56e50bde6UL, 0xf4576d13b684678bUL, 0xfdba8845fc0246a7UL, 0x2UL});
    ASSERT(!result.is_negative());
  }

  {
    BigInt left({0xd2ec154f6f077e95UL, 0x1bb578fcbf5bec60UL, 0x36735d365fe0f69eUL, 0xa803e03015c610e8UL, 0xca14aac782de50a2UL, 0x2191456abd619da2UL, 0xd9a345c56ba9a659UL, 0x1de31271dc3UL}, true);
    BigInt right({0xe2b272a002612fe3UL, 0xd1ec655e5e2e3d38UL, 0x21bc7b47bae7b899UL, 0xb211826c4e5c346fUL, 0x39d7cf09bfdfe04dUL, 0x1253ec5a1e5b8f9aUL, 0xbabcbf088154c6eeUL, 0x3e676f5f2637b2edUL, 0xcc7edfb347fcd8b4UL, 0x5978260d5ecf00a7UL, 0xe242586be49c40c1UL, 0xf734798ec1dd4ddaUL, 0x76394dUL}, true);
    BigInt result = left + right;
    check_contents(result, {0xb59e87ef7168ae78UL, 0xeda1de5b1d8a2999UL, 0x582fd87e1ac8af37UL, 0x5a15629c64224557UL, 0x3ec79d142be30f0UL, 0x33e531c4dbbd2d3dUL, 0x946004cdecfe6d47UL, 0x3e67713d575ed0b1UL, 0xcc7edfb347fcd8b4UL, 0x5978260d5ecf00a7UL, 0xe242586be49c40c1UL, 0xf734798ec1dd4ddaUL, 0x76394dUL});
    ASSERT(result.is_negative());
  }

}

void test_sub_1(TestObjs *objs) {
  // very basic tests for subtraction

  BigInt result1 = objs->zero - objs->zero;
  check_contents(result1, { 0UL });
  ASSERT(!result1.is_negative());

  BigInt result2 = objs->one - objs->zero;
  check_contents(result2, { 1UL });
  ASSERT(!result2.is_negative());

  // 0 - 1 should be -1
  BigInt result3 = objs->zero - objs->one;
  check_contents(result3, { 1UL });
  ASSERT(result3.is_negative());
}

void test_sub_2(TestObjs *) {
  // Harder test(s) for subtraction (with only nonnegative operands)
  {
    BigInt left({0x6eda4eff94cfbae7UL, 0x7c8c43da635c4344UL, 0x20cdb213d5541982UL, 0xbabfefb2192bd25fUL, 0x101fc2c8776f1e13UL, 0x91ecffb0a16ed1dfUL, 0x6f8e57b78c2b4126UL, 0x129a78f1cd7e3ce3UL, 0x64cb2c9c277276bUL, 0xd3e06abb451c73b8UL, 0xe9cc5fd7751987e1UL, 0x1a2d858e65ab0f23UL, 0x5cdd7cbf9bbb3853UL, 0xaac5e94bUL});
    BigInt right({0x555ecb5be6a2352UL, 0x6ac021fcd678e7bdUL, 0xb2c36b0123d8adefUL, 0xda6c10c4b0UL});
    BigInt result = left - right;
    check_contents(result, {0x69846249d6659795UL, 0x11cc21dd8ce35b87UL, 0x6e0a4712b17b6b93UL, 0xbabfeed7ad1b0daeUL, 0x101fc2c8776f1e13UL, 0x91ecffb0a16ed1dfUL, 0x6f8e57b78c2b4126UL, 0x129a78f1cd7e3ce3UL, 0x64cb2c9c277276bUL, 0xd3e06abb451c73b8UL, 0xe9cc5fd7751987e1UL, 0x1a2d858e65ab0f23UL, 0x5cdd7cbf9bbb3853UL, 0xaac5e94bUL});
    ASSERT(!result.is_negative());
  }
}

void test_sub_3(TestObjs *objs) {
  // Some mixed-sign subtraction tests

  BigInt result1 = objs->negative_three - objs->one;
  check_contents(result1, { 4UL });
  ASSERT(result1.is_negative());

  BigInt result2 = objs->one - objs->negative_three;
  check_contents(result2, { 4UL });
  ASSERT(!result2.is_negative());
}

void test_sub_4(TestObjs *) {
  // harder subtraction test(s) involving various
  // combinations of non-negative and negative operands

  {
    BigInt left({0xda069f1394210a58UL, 0x17aeeda0ba999cc9UL, 0x76d8613d4dfa6239UL, 0x2c20a77d76bb2b7dUL, 0xa852e8c7965c99cbUL, 0x2da25d942abf02ddUL, 0x8c7166619290106eUL, 0x72115b9db363aa85UL, 0xc3dcfa489c903019UL, 0x7f69b50321b7f3d5UL, 0x72ba467bb329ff89UL, 0x20a41c2UL}, true);
    BigInt right({0x25e9179995f954a7UL, 0x803da91345e6d0f3UL, 0x509d92c5fd3b8895UL, 0x2d61fc2da4c3962dUL, 0x1fe8e2e2223b1717UL, 0xe7e8724f74e51f4aUL, 0xe138b44b919a3926UL, 0xab9971d99b12be24UL, 0x4f39add59d016768UL, 0xe16b13e35c0e108cUL, 0xa6ef8ef9ec844222UL, 0xe78df73060cc35eeUL, 0x14fUL}, true);
    BigInt result = left - right;
    check_contents(result, {0x4be2788601d84a4fUL, 0x688ebb728b4d3429UL, 0xd9c53188af41265cUL, 0x14154b02e086aafUL, 0x7795fa1a8bde7d4cUL, 0xba4614bb4a261c6cUL, 0x54c74de9ff0a28b8UL, 0x3988163be7af139fUL, 0x8b5cb38d0071374fUL, 0x62015ee03a561cb6UL, 0x3435487e395a4299UL, 0xe78df7305ec1f42cUL, 0x14fUL});
    ASSERT(!result.is_negative());
  }

  {
    BigInt left({0x2731e5530045218aUL, 0x1c29aaUL});
    BigInt right({0xc2baed5ea8cf3dd0UL, 0x81fe5f9adde938d6UL, 0x56ec433719861074UL, 0x7f6d01dbc70c1274UL, 0xd02fd7f2403ee362UL, 0x24c5400e94347fb3UL, 0x764521caebba2c9UL, 0x51a61e0aa376ff6dUL, 0x103c5aa88eb6e90UL}, true);
    BigInt result = left - right;
    check_contents(result, {0xe9ecd2b1a9145f5aUL, 0x81fe5f9ade056280UL, 0x56ec433719861074UL, 0x7f6d01dbc70c1274UL, 0xd02fd7f2403ee362UL, 0x24c5400e94347fb3UL, 0x764521caebba2c9UL, 0x51a61e0aa376ff6dUL, 0x103c5aa88eb6e90UL});
    ASSERT(!result.is_negative());
  }
}

void test_is_bit_set_1(TestObjs *objs) {
  // some basic tests for is_bit_set

  ASSERT(!objs->zero.is_bit_set(0));
  ASSERT(!objs->zero.is_bit_set(1));

  // it should be ok to inquire about any bit position, even if it's beyond
  // the extent of the internal bitstring representation
  ASSERT(!objs->zero.is_bit_set(1000000));

  ASSERT(objs->one.is_bit_set(0));
  ASSERT(!objs->one.is_bit_set(1));

  ASSERT(!objs->two_pow_64.is_bit_set(63));
  ASSERT(objs->two_pow_64.is_bit_set(64));
  ASSERT(!objs->two_pow_64.is_bit_set(65));
}

void test_is_bit_set_2(TestObjs *) {
  // test(s) for is_bit_set: these test a random
  // sampling of bits in various large-ish BigInt values

  {
    BigInt val({0xad77cbed0273e33UL, 0xe151b7c18231a194UL, 0x7b2beb2888d66093UL, 0xeab20633a9a9595aUL});
    ASSERT(val.is_bit_set(0));
    ASSERT(val.is_bit_set(4));
    ASSERT(val.is_bit_set(9));
    ASSERT(!val.is_bit_set(15));
    ASSERT(!val.is_bit_set(24));
    ASSERT(val.is_bit_set(30));
    ASSERT(val.is_bit_set(42));
    ASSERT(val.is_bit_set(52));
    ASSERT(!val.is_bit_set(58));
    ASSERT(val.is_bit_set(89));
    ASSERT(val.is_bit_set(109));
    ASSERT(!val.is_bit_set(158));
    ASSERT(val.is_bit_set(177));
    ASSERT(val.is_bit_set(184));
    ASSERT(!val.is_bit_set(202));
    ASSERT(val.is_bit_set(241));
  }
}

void test_lshift_1(TestObjs *objs) {
  // Some very basic left shift tests

  BigInt result1 = objs->one << 1;
  check_contents(result1, { 2UL });
  ASSERT(!result1.is_negative());

  BigInt result2 = objs->one << 63;
  check_contents(result2, { 0x8000000000000000UL });
  ASSERT(!result2.is_negative());

  BigInt result3 = objs->one << 64;
  check_contents(result3, { 0UL, 1UL });
  ASSERT(!result3.is_negative());

  BigInt result4 = objs->three << 63;
  check_contents(result4, { 0x8000000000000000UL, 1UL });
  ASSERT(!result4.is_negative());

  // make sure that a std::invalid_argument exception is
  // thrown if there is an attempt to left shift a
  // negative value
  try {
    objs->negative_nine << 42;
    FAIL("left shifting a negative value should throw an exception");
  } catch (std::invalid_argument &ex) {
    // good
  }
}

void test_lshift_2(TestObjs *) {
  // left shift test(s) on large-ish values

  {
    BigInt val({0xbcc523fa26450fc2UL, 0x5490bb4c35ae6c03UL, 0x310a4f3349801bbeUL});
    BigInt result = val << 390;
    check_contents(result, {0x0UL, 0x0UL, 0x0UL, 0x0UL, 0x0UL, 0x0UL, 0x3148fe899143f080UL, 0x242ed30d6b9b00efUL, 0x4293ccd26006ef95UL, 0xcUL});
    ASSERT(!result.is_negative());
  }

  {
    BigInt val({0x7ca1c82cd5678c64UL, 0x24c995d549d6cbe8UL, 0x55df71ecab97c375UL, 0x9523341dc8fd0196UL, 0xf9c1dd3486f16b31UL, 0x7fe83598f38b19d1UL, 0x3b77ae13ce121dUL});
    BigInt result = val << 444;
    check_contents(result, {0x0UL, 0x0UL, 0x0UL, 0x0UL, 0x0UL, 0x0UL, 0x4000000000000000UL, 0x87ca1c82cd5678c6UL, 0x524c995d549d6cbeUL, 0x655df71ecab97c37UL, 0x19523341dc8fd019UL, 0x1f9c1dd3486f16b3UL, 0xd7fe83598f38b19dUL, 0x3b77ae13ce121UL});
    ASSERT(!result.is_negative());
  }
}

void test_mul_1(TestObjs *objs) {
  // some very basic multiplication tests

  BigInt result1 = objs->three * objs->three;
  check_contents(result1, { 9UL });
  ASSERT(!result1.is_negative());

  BigInt result2 = objs->two_pow_64 * objs->two;
  check_contents(result2, { 0UL, 2UL });
  ASSERT(!result2.is_negative());

  BigInt result3 = objs->u64_max * objs->u64_max;
  check_contents(result3, { 0x0000000000000001UL, 0xFFFFFFFFFFFFFFFEUL });
  ASSERT(!result3.is_negative());
}

void test_mul_2(TestObjs *) {
  // multiplication test(s) with larger values

  {
    BigInt left({0x63a2caa5324b539dUL, 0x176fde8a24cUL});
    BigInt right({0x7fd944a0481a66c6UL});
    BigInt result = left * right;
    check_contents(result, {0x2bf1cf198f85396eUL, 0x92c5b43447ed673fUL, 0xbb463828efUL});
    ASSERT(!result.is_negative());
  }
}

void test_compare_1(TestObjs *objs) {
  // some basic tests for compare
  ASSERT(objs->zero.compare(objs->zero) == 0);
  ASSERT(objs->zero.compare(objs->one) < 0);
  ASSERT(objs->one.compare(objs->zero) > 0);
  ASSERT(objs->negative_nine.compare(objs->three) < 0);
  ASSERT(objs->three.compare(objs->negative_three) > 0);
}

void test_compare_2(TestObjs *) {
  // test(s) for compare with larger values (both negative and
  // non-negative)

  {
    BigInt left({0xcedf54d4ce62e836UL, 0x80bcc908ac2470e5UL, 0x7f5530530656407UL, 0xd708e6b15caaffe6UL, 0xea4413128f8d85dfUL, 0x8761bb0238eef891UL, 0x112046064d41670bUL, 0x52e17af98118272fUL, 0x2bcb5a93df6b9791UL, 0xe4UL}, true);
    BigInt right({0xa4771be9296f10c9UL, 0x99652558ce608a70UL, 0x72a5a8d90da8a461UL, 0xae81eebacbc14061UL, 0xbe8ee1d7d75975c9UL, 0xef670eb972588d3aUL, 0x73c02c5138c02430UL, 0x6ed794ae35fd9f68UL, 0x4dd6b9ebe076ac72UL, 0x42b6daa3d05d11c7UL, 0x2cfe1d5965b49e99UL, 0x927becUL});
    int result = left.compare(right);
    ASSERT(result < 0);
  }
}

void test_div_1(TestObjs *objs) {
  // Some relatively basic division tests

  BigInt result1 = objs->nine / objs->three;
  check_contents(result1, { 3UL });
  ASSERT(!result1.is_negative());

  BigInt result2 = objs->nine / objs->two;
  check_contents(result2, { 4UL });
  ASSERT(!result2.is_negative());

  BigInt result3 = objs->negative_nine / objs->three;
  check_contents(result3, { 3UL });
  ASSERT(result3.is_negative());

  BigInt result4 = objs->negative_nine / objs->two;
  check_contents(result4, { 4UL });
  ASSERT(result4.is_negative());
}

void test_div_2(TestObjs *) {
  // harder division test(s) with larger values

  {
    BigInt left({0x5a1f7b06e95d205bUL, 0x16bef383084c9bf5UL, 0x6bfd5cb9a0cfa403UL, 0xbb47e519c0ffc392UL, 0xc8c47a8ab9cc20afUL, 0x30302fb07ef81d25UL, 0x8b8bcb6df3f72911UL, 0x3de679169dc89703UL, 0x48f52b428f255e1dUL, 0xd623c2e8a460f5beUL, 0xae2df81a84808054UL, 0xcfb038910d158d63UL, 0xcf97bc9UL});
    BigInt right({0xe1d191b09fd571e7UL, 0xd6e34973337d88fdUL, 0x7235628c33211b03UL, 0xe0bbc74b5d7fe26aUL, 0xf6242ed96eb2c8d9UL, 0x3b0cad8e5dd18f5UL, 0x558c283a839910c0UL, 0xbb4df9de72952652UL, 0xed8b519e3c63ce56UL, 0xe96f9c8454bde1c4UL, 0x76b62db592951f97UL, 0x577341UL});
    BigInt result = left / right;
    check_contents(result, {0xfb3e6b02be39b6ceUL, 0x25UL});
    ASSERT(!result.is_negative());
  }
}

void test_to_hex_1(TestObjs *objs) {
  // some basic tests for to_hex()

  std::string result1 = objs->zero.to_hex();
  ASSERT("0" == result1);

  std::string result2 = objs->negative_nine.to_hex();
  ASSERT("-9" == result2);

  std::string result3 = objs->u64_max.to_hex();
  ASSERT("ffffffffffffffff" == result3);

  std::string result4 = objs->two_pow_64.to_hex();
  ASSERT("10000000000000000" == result4);
}

void test_to_hex_2(TestObjs *) {
  // test(s) for to_hex() on larger values

  {
    BigInt val({0xd8b5422df2c7e5d4UL, 0x2186595636ed41d7UL, 0xcf498dc4c634eb41UL, 0xa6579a3f9d2aab0cUL, 0xb5cbefaf0e63a6e3UL, 0xf419b0aadf4d14f1UL, 0xcec650d523acc64eUL, 0x14318cf757a58UL});
    std::string result = val.to_hex();
    ASSERT("14318cf757a58cec650d523acc64ef419b0aadf4d14f1b5cbefaf0e63a6e3a6579a3f9d2aab0ccf498dc4c634eb412186595636ed41d7d8b5422df2c7e5d4" == result);
  }

  {
    BigInt val({0xd8b5422df2c7e5d4UL, 0x2186595636ed41d7UL, 0xcf498dc4c634eb41UL, 0xa6579a3f9d2aab0cUL, 0xb5cbefaf0e63a6e3UL, 0xf419b0aadf4d14f1UL, 0xcec650d523acc64eUL, 0x14318cf757a58UL}, true);
    std::string result = val.to_hex();
    ASSERT("-14318cf757a58cec650d523acc64ef419b0aadf4d14f1b5cbefaf0e63a6e3a6579a3f9d2aab0ccf498dc4c634eb412186595636ed41d7d8b5422df2c7e5d4" == result);
  }

}

void test_to_dec_1(TestObjs *objs) {
  // some basic tests for to_dec()
  std::string result1 = objs->three.to_dec();
  ASSERT("3" == result1);

  std::string result2 = objs->negative_nine.to_dec();
  ASSERT("-9" == result2);

  std::string result3 = objs->u64_max.to_dec();
  ASSERT("18446744073709551615" == result3);
}

void test_to_dec_2(TestObjs *) {
  // harder test(s) for to_dec()
  {
    BigInt val({0x361adeb15b6962c7UL, 0x31a5b3c012d2a685UL, 0x7b3b4839UL});
    std::string result = val.to_dec();
    ASSERT("703527900324720116021349050368162523567079645895" == result);
  }
}

void hw_1_to_hex_tests(TestObjs *objs){
  std::string result1 = objs->zero.to_hex();
  ASSERT("0" == result1);

  std::string result2 = objs->two_pow_64_plus_one.to_hex();
  ASSERT("10000000000000001" == result2);

  std::string result3 = objs->two_pow_64_plus_hex.to_hex();
  ASSERT("10000000000000101" == result3);

  std::string result4 = objs->multiple_zeros.to_hex();
  ASSERT("0" == result4);

}

void hw_1_unary_is_negative_tests(TestObjs *objs){
  BigInt negative_three = -(objs->three);
  ASSERT(negative_three.is_negative());  

  BigInt negative_zero = -objs->zero; //"zero" initialized false for negative
  ASSERT(!negative_zero.is_negative()); //unary negation should have no effect

  BigInt negative_mult_zeros = -objs->multiple_zeros; 
  ASSERT(!negative_mult_zeros.is_negative());

  BigInt negative_negative_three = -(-(objs->three));
  ASSERT(!negative_negative_three.is_negative());

  BigInt two_to_pow_64 = -(objs->negative_two_pow_64);
  ASSERT(!two_to_pow_64.is_negative());

}

void hw1_get_bits_get_bit_vector_tests(TestObjs *objs){
  ASSERT(3UL == objs->three.get_bits(0));
  ASSERT(0UL == objs->three.get_bits(1));
  ASSERT(objs->three.get_bit_vector() == objs->negative_three.get_bit_vector());

  ASSERT(5UL == objs->really_big_number.get_bits(8));
  ASSERT(2UL == objs->really_big_number.get_bits(7));
  ASSERT(1UL == objs->really_big_number.get_bits(6));
  ASSERT(3UL == objs->really_big_number.get_bits(5));
  ASSERT(0UL == objs->really_big_number.get_bits(4));
  ASSERT(9UL == objs->really_big_number.get_bits(3));
  ASSERT(6UL == objs->really_big_number.get_bits(2));
  ASSERT(7UL == objs->really_big_number.get_bits(1));
  ASSERT(4UL == objs->really_big_number.get_bits(0));
  ASSERT(0UL == objs->really_big_number.get_bits(9));
  ASSERT((objs->really_big_number.get_bit_vector() == std::vector<uint64_t>{4UL, 7UL, 6UL, 9UL, 0UL, 3UL, 1UL, 2UL, 5UL}));

  ASSERT(0UL == objs->multiple_zeros.get_bits(0));
  ASSERT(0UL == objs->multiple_zeros.get_bits(4));
  ASSERT((objs->multiple_zeros.get_bit_vector() == std::vector<uint64_t>{0, 0, 0}));;

  ASSERT(257UL == objs->two_pow_64_plus_hex.get_bits(0));
  ASSERT(1UL == objs->two_pow_64_plus_hex.get_bits(1));
  ASSERT((objs->two_pow_64_plus_hex.get_bit_vector() == std::vector<uint64_t>{257UL, 1UL}));
}

void hw1_constructors_equals_tests(TestObjs *objs){
  BigInt result1 = objs->two_pow_64_plus_hex;
  ASSERT(result1.get_bit_vector() == objs->two_pow_64_plus_hex.get_bit_vector());
  ASSERT(result1.is_negative() == objs->two_pow_64_plus_hex.is_negative());

  BigInt result2 = objs->multiple_zeros;
  ASSERT(result2.get_bit_vector() == objs->multiple_zeros.get_bit_vector());
  ASSERT(result2.is_negative() == objs->multiple_zeros.is_negative());

  BigInt result3 = BigInt(objs->really_big_number);
  ASSERT(result3.get_bit_vector() == objs->really_big_number.get_bit_vector());
  ASSERT(result3.is_negative() == objs->really_big_number.is_negative());

  BigInt result4 = BigInt(48UL, true);
  ASSERT(result4.get_bits(0) == 48UL);
  ASSERT(result4.is_negative());
}

void hw1_addition_tests(TestObjs *objs) {

// 0 + 2 = 2
ASSERT(objs->zero + objs->two == objs->two);

// 0 - 3 = -3
ASSERT(objs->zero - objs->three == objs->negative_three);

// 3 + (-3) = 0
ASSERT(objs->three + objs->negative_three == objs->zero);

// -3 + 3 = 0
ASSERT(objs->negative_three + objs->three == objs->zero);

// 0 + 0 = 0
ASSERT(objs->zero + objs->zero == objs->zero);

//Edge case: Spillover into the next vector index
ASSERT(objs->u64_max + objs->one == objs->two_pow_64);

//2^64 + 2^64 = 2^65
ASSERT(objs->two_pow_64 + objs->two_pow_64 == objs->two_pow_65);


}

void hw1_subtract_tests(TestObjs *objs) {}

void hw1_multiply_tests(TestObjs *objs) {

// Test multiplication with zero
    ASSERT((objs->zero * objs->one) == objs->zero);           // 0 * 1 = 0
    ASSERT((objs->one * objs->zero) == objs->zero);           // 1 * 0 = 0

    // Test multiplication with one
    ASSERT((objs->one * objs->one) == objs->one);             // 1 * 1 = 1
    ASSERT((objs->one * objs->two) == objs->two);             // 1 * 2 = 2
    ASSERT((objs->two * objs->one) == objs->two);             // 2 * 1 = 2

    // Test multiplication of positive numbers
    ASSERT((objs->three * objs->three) == objs->nine);        // 3 * 3 = 9
    
    // Test multiplication with large numbers
    ASSERT((objs->u64_max * objs->one) == objs->u64_max);     // UINT64_MAX * 1 = UINT64_MAX
    ASSERT((objs->u64_max * objs->two) == objs->u64_max * 2); // UINT64_MAX * 2 = UINT64_MAX * 2
    
    // Test multiplication with negative numbers
    ASSERT((objs->negative_three * objs->three) == objs->negative_nine); // -3 * 3 = -9
    
    // Test multiplication with large negative numbers
    ASSERT((objs->negative_two_pow_64 * objs->one) == objs->negative_two_pow_64); // -2^64 * 2 = -2^65
    
    // Test multiplication of large number with zero
    ASSERT((objs->multiple_zeros * objs->really_big_number) == objs->zero); // 0 * very_large_number = 0
    ASSERT((objs->really_big_number * objs->multiple_zeros) == objs->zero); // very_large_number * 0 = 0

}

void hw1_divide_tests(TestObjs *objs) {
  
  //Mixed sign division
  ASSERT(objs->nine / objs->negative_three == objs->negative_three); 
  ASSERT(objs->negative_nine / objs->three == objs->negative_three); 

  //Both negative
  ASSERT(objs->negative_nine / objs->negative_three == objs->three); 

  //Integer division where dividend < divisor
  ASSERT(objs->three / objs->nine == objs->zero); 
  ASSERT(objs->negative_three / objs->nine == objs->zero);

}

void hw1_is_bit_set_tests(TestObjs *objs) {
  //Testing big number and returning 0 for indices that are outside the number.
  BigInt new_val({1311768467463790320UL, 1311747204760522223UL, 14974415777481871311UL});
  ASSERT(!new_val.is_bit_set(0));
  ASSERT(new_val.is_bit_set(9));
  ASSERT(new_val.is_bit_set(14));
  ASSERT(new_val.is_bit_set(60));
  ASSERT(!new_val.is_bit_set(61));
  ASSERT(!new_val.is_bit_set(62));  
  ASSERT(!new_val.is_bit_set(63)); 
  ASSERT(new_val.is_bit_set(64));
  ASSERT(!new_val.is_bit_set(68));
  ASSERT(new_val.is_bit_set(82));
  ASSERT(new_val.is_bit_set(124));
  ASSERT(!new_val.is_bit_set(125));
  ASSERT(!new_val.is_bit_set(126));  
  ASSERT(!new_val.is_bit_set(127)); 
  ASSERT(new_val.is_bit_set(128)); 
  ASSERT(!new_val.is_bit_set(132)); 
  ASSERT(!new_val.is_bit_set(140)); 
  ASSERT(new_val.is_bit_set(191)); 
  ASSERT(new_val.is_bit_set(190)); 
  ASSERT(!new_val.is_bit_set(189)); 
  ASSERT(!new_val.is_bit_set(192));   
  ASSERT(!new_val.is_bit_set(500));

  // Ensuring that these results are the same for a negative BigInt
  BigInt neg_val({1311768467463790320UL, 1311747204760522223UL, 14974415777481871311UL}, true);
  ASSERT(!new_val.is_bit_set(0));
  ASSERT(new_val.is_bit_set(9));
  ASSERT(new_val.is_bit_set(14));
  ASSERT(new_val.is_bit_set(60));
  ASSERT(!new_val.is_bit_set(61));
  ASSERT(!new_val.is_bit_set(62));  
  ASSERT(!new_val.is_bit_set(63)); 
  ASSERT(new_val.is_bit_set(64));
  ASSERT(!new_val.is_bit_set(68));
  ASSERT(new_val.is_bit_set(82));
  ASSERT(new_val.is_bit_set(124));
  ASSERT(!new_val.is_bit_set(125));
  ASSERT(!new_val.is_bit_set(126));  
  ASSERT(!new_val.is_bit_set(127)); 
  ASSERT(new_val.is_bit_set(128)); 
  ASSERT(!new_val.is_bit_set(132)); 
  ASSERT(!new_val.is_bit_set(140)); 
  ASSERT(new_val.is_bit_set(191)); 
  ASSERT(new_val.is_bit_set(190)); 
  ASSERT(!new_val.is_bit_set(189)); 
  ASSERT(!new_val.is_bit_set(192));   
  ASSERT(!new_val.is_bit_set(500));

  //Testing for edge case: Multiple zeros are added in an array
  ASSERT(!objs->multiple_zeros.is_bit_set(0));
  ASSERT(!objs->multiple_zeros.is_bit_set(1));
  ASSERT(!objs->multiple_zeros.is_bit_set(2));
  ASSERT(!objs->multiple_zeros.is_bit_set(3));
  ASSERT(!objs->multiple_zeros.is_bit_set(4));

  //Testing for edge case: BigInt value of 1
  ASSERT(objs->one.is_bit_set(0));
  ASSERT(!objs->one.is_bit_set(1));

  //Testing for edge case: BigInt value of 2^64
  for(int i = 0; i < 64; i++){
    ASSERT(!objs->two_pow_64.is_bit_set(i));
  }
  ASSERT(objs->two_pow_64.is_bit_set(64));

  //Testing for edge case: BigInt value of u64Max
  for(int i = 0; i < 64; i++){
    ASSERT(objs->u64_max.is_bit_set(i));
  }

  BigInt u64_max_rep({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL});
  for(int i = 0; i < 64*7; i++){
    ASSERT(u64_max_rep.is_bit_set(i));
  }
  ASSERT(!u64_max_rep.is_bit_set(64*7));
}

void hw1_compare_tests(TestObjs *objs) {
  //  Edge Case Tests: Compare -1 and 1; compare -1 with itself; compare -1 with the same value but different object
  BigInt other_neg_one = BigInt({1}, true);
  ASSERT(objs->negative_one.compare(objs->one) < 0);
  ASSERT(objs->negative_one.compare(objs->zero) < 0);
  ASSERT(objs->negative_one.compare(objs->negative_three) > 0);
  ASSERT(objs->negative_one.compare(objs->negative_one) == 0);
  ASSERT(objs->negative_one.compare(other_neg_one) == 0);

  //Edge Cases: Compare BigInts with different vector sizes, but smaller vector having larger values in each index 
  BigInt larger = BigInt({0, 0, 1});
  BigInt smaller = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL});
  ASSERT(larger.compare(smaller) > 0);
  ASSERT(smaller.compare(larger) < 0);

  //Edge Cases: Compare same magnitudes as above, but making the values negative. Also, ensure that negative values work.
  BigInt larger_neg = BigInt({0, 0, 1}, true);
  BigInt smaller_neg = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL}, true);
  ASSERT(larger_neg.compare(smaller_neg) < 0);
  ASSERT(smaller_neg.compare(larger_neg) > 0);
  ASSERT(larger_neg.compare(smaller) < 0);
  
  //Edge Cases: Compare BigInts with vectors largely the same except for a difference in the smallest index.
  BigInt larger_same = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL});
  BigInt smaller_same = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFEUL});
  ASSERT(larger_same.compare(smaller_same) > 0);
  ASSERT(smaller_same.compare(larger_same) < 0);

  //Edge Cases: Ensure that adding more zeros to the end of a number vector does not change the magnitude
  ASSERT(objs->multiple_zeros.compare(objs->zero) == 0);

  BigInt base = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL});
  BigInt base_more_zeros = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0, 0, 0});
  ASSERT (base.compare(base_more_zeros) == 0);

  BigInt base_neg = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL});
  BigInt base_more_zeros_neg = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0, 0, 0});
  ASSERT (base_neg.compare(base_more_zeros_neg) == 0);

  BigInt smaller_number_more_zeros = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0, 0});
  BigInt larger_number_less_zeros = BigInt({0, 0, 1});
  ASSERT (larger_number_less_zeros.compare(smaller_number_more_zeros) > 0);

  BigInt smaller_number_more_zeros_neg = BigInt({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0, 0}, true);
  BigInt larger_number_less_zeros_neg = BigInt({0, 0, 1, 0, 0, 0}, true);
  ASSERT (larger_number_less_zeros_neg.compare(smaller_number_more_zeros_neg) < 0);
}

void hw1_left_shift_tests(TestObjs *objs) {
  //See if shifting by 1 
  BigInt result1 = objs->one << 0;
  ASSERT(result1.get_bit_vector() == objs->one.get_bit_vector());
  ASSERT(!result1.is_negative());

  BigInt result2 = objs->two_pow_64 << 64;
  BigInt exp_res_2 = BigInt({0UL, 0UL, 1UL});
  ASSERT(result2.get_bit_vector() == exp_res_2.get_bit_vector());
  ASSERT(!result2.is_negative());

  //Ensure all bits shift left
  BigInt result3 = objs->two_pow_64_plus_one << 64;
  BigInt exp_res_3 = BigInt({0UL, 1UL, 1UL});
  ASSERT(result3.get_bit_vector() == exp_res_3.get_bit_vector());
  ASSERT(!result3.is_negative());

  //Comparing that zero left shifted is zero
  BigInt result4 = objs->zero << 12347348;
  ASSERT(result4.compare(objs->zero) == 0);
  ASSERT(!result4.is_negative());

  BigInt result5 = objs->multiple_zeros << 12347348;
  ASSERT(result5.compare(objs->multiple_zeros) == 0);
  ASSERT(!result5.is_negative());

  //Comparing that the formula of multiplied by 2 per bit shift is followed
  BigInt thirty_seven = BigInt({37});
  BigInt result6 = thirty_seven << 37;
  BigInt exp_res_6 = BigInt(5085241278464, false);
  ASSERT(result6.compare(exp_res_6) == 0);
  ASSERT(!result6.is_negative());

  //Check to see if std::invalid_argument is thrown for negative one
  try {
    objs->negative_one << 0;
    FAIL("left shifting a negative value should throw an exception");
  } catch (std::invalid_argument &ex) {
    // GOOD CATCH!
  }
  
}

void hw1_to_dec_tests(TestObjs *objs) {
//Edge Case: Test if 0 prints correctly for multiple zeros and negative zero
  ASSERT(objs->zero.to_dec() == "0");
  ASSERT(objs->multiple_zeros.to_dec() == "0");
  BigInt neg_zero_unfilled = BigInt({}, true);
  BigInt neg_zero_filled = BigInt({0}, true);
  ASSERT(neg_zero_unfilled.to_dec() == "0");
  ASSERT(neg_zero_filled.to_dec() == "0");

  //Edge Case: Test if 0s in front of a number have it print correctly
  BigInt val({0x361adeb15b6962c7UL, 0x31a5b3c012d2a685UL, 0x7b3b4839UL, 0UL, 0UL, 0UL, 0UL, 0UL});
  std::string result = val.to_dec();
  ASSERT("703527900324720116021349050368162523567079645895" == result);

  //Edge Case: Test if 0s in front of a number have it print correctly
  BigInt val_neg({0x361adeb15b6962c7UL, 0x31a5b3c012d2a685UL, 0x7b3b4839UL, 0UL, 0UL, 0UL, 0UL, 0UL}, true);
  std::string result_neg = val_neg.to_dec();
  ASSERT("-703527900324720116021349050368162523567079645895" == result_neg);

  //Testing a really large number
  BigInt big_num({0x7f3c9d2a14b8e5f1UL, 0xa6d2e3f04c9b7a8dUL, 0x0e6f4c9a1b2d8e3fUL, 0x4a6b7c8d9e0f1a2bUL, 0x7f3c9d2a14b8e5f1UL, 0xa6d2e3f04c9b7a8dUL});
  std::string result2 = big_num.to_dec();
  ASSERT("25676531365894721546215871327825460310544539604088347369401678692335220609481358959798999890609518318106951215015409" == result2);

  //Testing a really large number that is negative
  BigInt big_num_neg({0x7f3c9d2a14b8e5f1UL, 0xa6d2e3f04c9b7a8dUL, 0x0e6f4c9a1b2d8e3fUL, 0x4a6b7c8d9e0f1a2bUL, 0x7f3c9d2a14b8e5f1UL, 0xa6d2e3f04c9b7a8dUL}, true);
  std::string result2_neg = big_num_neg.to_dec();
  ASSERT("-25676531365894721546215871327825460310544539604088347369401678692335220609481358959798999890609518318106951215015409" == result2_neg);

  //Testing multiple maxed out indices in the vector
  BigInt u64_max_rep({0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL});
  std::string result3 = u64_max_rep.to_dec();
  ASSERT("340282366920938463463374607431768211455" == result3);

  //Testing for sanity
  BigInt thirty_two({32}, true);
  std::string result4 = thirty_two.to_dec();
  ASSERT("-32" == result4);

  //Testing numbers that have 0 in their first index -- Caused a change in to_dec()
  std::string result5 = objs->two_pow_64.to_dec();
  ASSERT("18446744073709551616" == result5);

  BigInt neg_two_pow_64({0, 1}, true);
  std::string result6 = neg_two_pow_64.to_dec();
  ASSERT("-18446744073709551616" == result6);

  BigInt two_pow_128({0, 0, 1}); 
  std::string result7 = two_pow_128.to_dec();
  ASSERT("340282366920938463463374607431768211456" == result7);

  BigInt two_pow_128_neg({0, 0, 1}, true); 
  std::string result8 = two_pow_128_neg.to_dec();
  ASSERT("-340282366920938463463374607431768211456" == result8);
}