#include <gtest/gtest.h>

#include <cstring>
#include <vector>

#include "HttpMessage.hpp"

class HttpMessageTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}

  void ExpectHeader(HttpMessage& message, const std::string& name,
                    const std::string& value) {
    EXPECT_EQ(value, message.GetHeader(name));
  }

  void ExpectBody(HttpMessage& message, std::vector<unsigned char>& body) {
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
  const char* first = "hello, world\n";
  const char* second = "this is test\n";
  const char* third = "end of message\n";
  std::vector<unsigned char> first_bytes(first, first + strlen(first));
  std::vector<unsigned char> second_bytes(second, second + strlen(second));
  std::vector<unsigned char> third_bytes(third, third + strlen(third));
  message.AppendBody(first_bytes);
  message.AppendBody(second_bytes);
  message.AppendBody(third_bytes);

  std::vector<unsigned char> answer;
  answer.insert(answer.end(), first, first + strlen(first));
  answer.insert(answer.end(), second, second + strlen(second));
  answer.insert(answer.end(), third, third + strlen(third));
  ExpectBody(message, answer);
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
