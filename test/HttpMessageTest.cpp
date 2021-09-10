#include <gtest/gtest.h>

#include "HttpMessage.hpp"

class HttpMessageTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}

  void ExpectHeader(HttpMessage& message, const std::string& name,
                    const std::string& value) {
    EXPECT_EQ(value, message.GetHeader(name));
  }

  void ExpectBody(HttpMessage& message, const std::string& body) {
    std::vector<unsigned char> body_bytes(body.begin(), body.end());
    EXPECT_EQ(body_bytes, message.GetBody());
  }
};

TEST_F(HttpMessageTest, Hedear) {
  HttpMessage message;
  message.AppendHeader("name-type", "value");
  ExpectHeader(message, "name-type", "value");
  ExpectHeader(message, "Name-type", "value");
  ExpectHeader(message, "nAme-TYpe", "value");
  ExpectHeader(message, "naMe-type", "value");
  ExpectHeader(message, "namE-tYpe", "value");
  ExpectHeader(message, "NAme-Type", "value");
  ExpectHeader(message, "nAMe-type", "value");
  ExpectHeader(message, "naME-tyPe", "value");
  ExpectHeader(message, "NamE-typE", "value");
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

TEST_F(HttpMessageTest, Date) {
  HttpMessage message;
  time_t now = std::time(NULL);
  char buf[80];
  std::strftime(buf, sizeof(buf), "%a, %b %Y %H:%M:%S GMT", std::gmtime(&now));
  std::string date = message.Now(now);
  EXPECT_STREQ(buf, date.c_str());
}

TEST_F(HttpMessageTest, Exception) {
  HttpMessage message;
  message.AppendHeader("name", "value");
  try {
    message.GetHeader("aaa");
  } catch (HttpMessage::HeaderKeyException& e) {
    SUCCEED();
    return;
  }
  FAIL();
}
