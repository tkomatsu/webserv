#include <gtest/gtest.h>
#include <vector>
#include "Request.hpp"

class RequestTest : public testing::Test {
protected:
  virtual void SetUp() {
    requests.push_back("GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
    requests.push_back("POST / HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
    requests.push_back("Host:www.google.com\r\n\r\n");
    requests.push_back("PUT / HTTP/1.3\r\nHost: www.google.com\r\n\r\n");
    requests.push_back("DELETE / HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
    requests.push_back("HEAD / HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
    requests.push_back("GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\nhello world. this isa test.\r\n");
  }
  virtual void TearDown() {}

  std::vector<std::string> requests;
};

TEST_F(RequestTest, Standard) {
  Request request;
  request.AppendRawData(requests[0]);
  EXPECT_EQ(request.GetMethod(), GET);
  EXPECT_EQ(request.GetURI(), "/");
  EXPECT_EQ(request.GetVersion(), "1.1");
  EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
  EXPECT_EQ(request.GetBody() ,"");
}