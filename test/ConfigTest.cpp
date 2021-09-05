#include <gtest/gtest.h>

#include "config.hpp"

class LineBuilderTest : public testing::Test {
  protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST(LineBuilderTest, TestEmpty) {
  std::ifstream file("../../conf/empty.conf");
  config::LineBuilder builder(file);
  config::LineComponent line;

  EXPECT_FALSE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::NONE);
}

TEST(LineBuilderTest, TestWhitespace) {
  std::ifstream file("../../conf/whitespaces.conf");
  config::LineBuilder builder(file);
  config::LineComponent line;

  EXPECT_FALSE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::NONE);
}

class ConfigTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST(ConfigTest, TestTest) {
  config::Parser parser("../../conf/default.conf");
  std::vector<config::Config> configs = parser.GetConfigs();
}