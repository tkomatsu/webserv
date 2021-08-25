#include <gtest/gtest.h>

#include <regex>

#include "Response.hpp"

class ResponseTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}

  void ExpectVersion(Response& res, const std::string& version) {
    EXPECT_STREQ(res.GetVersion().c_str(), version.c_str());
  }
};

TEST_F(ResponseTest, Version) {
  Response res;
  ExpectVersion(res, "1.1");
}

TEST_F(ResponseTest, AutoIndex) {
  Response res;
  res.AutoIndexResponse("./");
  EXPECT_TRUE(std::regex_match(res.GetBody(), std::regex("(.|\n)*")));
}
