#include <gtest/gtest.h>

#include "utility.hpp"

class utilityTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(utilityTest, Split) {
  std::string a = ft::ltoa(42);

  EXPECT_EQ(a, "42");
  a = ft::ltoa(2147483648);
  EXPECT_EQ(a, "2147483648");
  a = ft::ltoa(2147483647);
  EXPECT_EQ(a, "2147483647");
  a = ft::ltoa(-2147483648);
  EXPECT_EQ(a, "-2147483648");
  a = ft::ltoa(-2147483649);
  EXPECT_EQ(a, "-2147483649");
}
