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

// autoindex test
TEST_F(ResponseTest, AutoIndex) {
  Response res;
  res.AutoIndexResponse("./", "./");
  //  EXPECT_TRUE(std::regex_match(res.GetBody(), std::regex("(.|\n)*")));
  EXPECT_TRUE(res.GetStatusCode() == 200);
  EXPECT_TRUE(res.GetStatusMessage() == "OK");
  EXPECT_TRUE(res.GetVersion() == "1.1");
  EXPECT_TRUE(res.GetHeader("Content-Type") == "text/html");
}

// Error Response test
TEST_F(ResponseTest, Response404) {
  Response res;
  res.ErrorResponse(404);
  //  EXPECT_TRUE(std::regex_match(res.GetBody(), std::regex("(.|\n)*")));
  EXPECT_TRUE(res.GetStatusCode() == 404);
  EXPECT_TRUE(res.GetStatusMessage() == "Not Found");
  EXPECT_TRUE(res.GetVersion() == "1.1");
  EXPECT_TRUE(res.GetHeader("Content-Type") == "text/html");
}

TEST_F(ResponseTest, Response405) {
  Response res;
  res.ErrorResponse(405);
  //  EXPECT_TRUE(std::regex_match(res.GetBody(), std::regex("(.|\n)*")));
  EXPECT_TRUE(res.GetStatusCode() == 405);
  EXPECT_TRUE(res.GetStatusMessage() == "Method Not Allowd");
  EXPECT_TRUE(res.GetVersion() == "1.1");
  EXPECT_TRUE(res.GetHeader("Content-Type") == "text/html");
}

TEST_F(ResponseTest, Response502) {
  Response res;
  res.ErrorResponse(502);
  //  EXPECT_TRUE(std::regex_match(res.GetBody(), std::regex("(.|\n)*")));
  EXPECT_TRUE(res.GetStatusCode() == 502);
  EXPECT_TRUE(res.GetStatusMessage() == "Bad Gateway");
  EXPECT_TRUE(res.GetVersion() == "1.1");
  EXPECT_TRUE(res.GetHeader("Content-Type") == "text/html");
}

TEST_F(ResponseTest, Response503) {
  Response res;
  res.ErrorResponse(503);
  //  EXPECT_TRUE(std::regex_match(res.GetBody(), std::regex("(.|\n)*")));
  EXPECT_TRUE(res.GetStatusCode() == 503);
  EXPECT_TRUE(res.GetStatusMessage() == "Service Unavailable");
  EXPECT_TRUE(res.GetVersion() == "1.1");
  EXPECT_TRUE(res.GetHeader("Content-Type") == "text/html");
}
