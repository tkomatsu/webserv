#include <gtest/gtest.h>

#include "utility.hpp"

class utilityTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}

  void ExpectResult(long num) {
    std::string exp = std::to_string(num);
    std::string ret = ft::ltoa(num);
    EXPECT_STREQ(ret.c_str(), exp.c_str());
  }
};

TEST_F(utilityTest, Ltoa) {
  ExpectResult(2147483648);
  ExpectResult(2147483647);
  ExpectResult(-2147483648);
  ExpectResult(-2147483649);
  ExpectResult(42);
  ExpectResult(-42);
  ExpectResult(0);
}
