#include <gtest/gtest.h>
#include "Request.hpp"

class RequestTest : public testing::Test {
protected:
  virtual void SetUp() {
    std::string str1 = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    std::string str2 = "POST / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    std::string str3 = "Host:www.google.com\r\n\r\n";
    std::string str4 = "PUT / HTTP/1.3\r\nHost: www.google.com\r\n\r\n";
    std::string str5 = "DELETE / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    std::string str6 = "HEAD / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    std::string str7 = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\nhello world. this isa test.\r\n";
  }
  virtual void TearDown() {}
};

TEST_F(RequestTest, Standard) {
  Request request;
  std::string str1 = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
  request.AppendRawData(str1);
  EXPECT_EQ(request.GetMethod(), GET);
  EXPECT_EQ(request.GetURI(), "/");
  EXPECT_EQ(request.GetVersion(), "1.1");
//  EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
  EXPECT_EQ(request.GetBody() ,"");
}