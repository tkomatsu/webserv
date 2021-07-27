#include <gtest/gtest.h>
#include <map>
#include "Request.hpp"

class RequestTest : public testing::Test {
protected:
  virtual void SetUp() {
    requests["standard"] = "GET / HTTP/1.1\r\nHost: www.google.com\r\nDate: Mon, 1 Jan 2010 01:01:01 GMT\r\n\r\n";
    requests["post"] = "POST / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["delete"] = "DELETE / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["put"] = "PUT / HTTP/1.3\r\nHost: www.google.com\r\n\r\n";
    requests["head"] = "HEAD / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["invalid"] = "Host:www.google.com\r\n\r\n";
    requests["body"] = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\nhello world. this isa test.\r\n";
    requests["chunk"] = "POST / HTTP/1.1\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\n\r\n7\r\nMozilla\r\n9\r\nDeveloper\r\n7\r\nNetwork\r\n0\r\n\r\n";
  }
  virtual void TearDown() {}

  std::map<std::string, std::string> requests;
};

TEST_F(RequestTest, Standard) {
  Request request;
  request.AppendRawData(requests["standard"]);
  EXPECT_EQ(request.GetMethod(), GET);
  EXPECT_EQ(request.GetURI(), "/");
  EXPECT_EQ(request.GetVersion(), "1.1");
  EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
  EXPECT_EQ(request.GetHeader("Date"), "Mon, 1 Jan 2010 01:01:01 GMT");
  EXPECT_EQ(request.GetBody() ,"");
}

TEST_F(RequestTest, Chunked) {
  Request request;
  request.AppendRawData(requests["chunk"]);
  EXPECT_EQ(request.GetMethod(), POST);
  EXPECT_EQ(request.GetURI(), "/");
  EXPECT_EQ(request.GetVersion(), "1.1");
  EXPECT_EQ(request.GetHeader("Content-Type"), "text/plain");
  EXPECT_EQ(request.GetHeader("Transfer-Encoding"), "chunked");
  EXPECT_EQ(request.GetBody(), "MozillaDeveloperNetwork");
}