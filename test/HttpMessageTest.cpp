#include <gtest/gtest.h>
#include "HttpMessage.hpp"

class HttpMessageTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}

  void ExpectHeader(HttpMessage &message, const std::string& name, const std::string& value) {
    EXPECT_EQ(value, message.GetHeader(name));
  }

  void ExpectBody(HttpMessage &message, const std::string& body) {
    EXPECT_EQ(body, message.GetBody());
  }

};

TEST_F(HttpMessageTest, Hedears) {
  HttpMessage message;
  message.AppendHeader("name", "value");
  ExpectHeader(message, "name", "value");
}

TEST_F(HttpMessageTest, Body) {
  HttpMessage message;
  std::string first = "hello, world\n";
  std::string second = "this is test\n";
  std::string third = "end of message\n";
  message.AppendBody(first);
  message.AppendBody(second);
  message.AppendBody(third);
  ExpectBody(message, first + second + third);
}