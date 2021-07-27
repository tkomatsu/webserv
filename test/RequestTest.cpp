#include <gtest/gtest.h>
#include <map>
#include "Request.hpp"

class RequestTest : public testing::Test {
protected:
  virtual void SetUp() {
    requests["get"] = "GET / HTTP/1.1\r\nHost: www.google.com\r\nDate: Mon, 1 Jan 2010 01:01:01 GMT\r\nContent-Length: 0\r\n\r\n";
    requests["post"] = "POST / HTTP/1.1\r\nHost: www.google.com\r\nContent-Length: 0\r\n\r\n";
    requests["delete"] = "DELETE / HTTP/1.1\r\nHost: www.google.com\r\nContent-Length: 0\r\n\r\n";
    requests["put"] = "PUT / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["head"] = "HEAD / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["invalid"] = "Host:www.google.com\r\n\r\n";
    requests["no length"] = "GET / HTTP/1.1\r\nHost: www.google.com\r\nDate: Mon, 1 Jan 2010 01:01:01 GMT\r\n\r\n";
    requests["body"] = "POST / HTTP/1.1\r\nHost: www.google.com\r\nContent-Length: 27\r\n\r\nhello world. this isa test.";
    requests["chunk"] = "POST / HTTP/1.1\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\n\r\n7\r\nMozilla\r\n9\r\nDeveloper\r\n7\r\nNetwork\r\n0\r\n\r\n";
  }
  virtual void TearDown() {}

  std::map<std::string, std::string> requests;
};

TEST_F(RequestTest, Standard) {
  {
    Request request;
    request.AppendRawData(requests["get"]);
    EXPECT_EQ(request.GetMethod(), GET);
    EXPECT_EQ(request.GetURI(), "/");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
    EXPECT_EQ(request.GetHeader("Date"), "Mon, 1 Jan 2010 01:01:01 GMT");
    EXPECT_EQ(request.GetBody() ,"");
  }
  {
    Request request;
    request.AppendRawData(requests["post"]);
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
    EXPECT_EQ(request.GetBody(), "");
  }
  {
    Request request;
    request.AppendRawData(requests["delete"]);
    EXPECT_EQ(request.GetMethod(), DELETE);
    EXPECT_EQ(request.GetURI(), "/");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
    EXPECT_EQ(request.GetBody(), "");
  }
}

TEST_F(RequestTest, Invalid_method) {
  {
    Request request;
    try {
      request.AppendRawData(requests["put"]);
      FAIL();
    } catch (Request::RequestFatalException &e) {
      EXPECT_STREQ(e.what(), "Invalid method");
    }
  }
  {
    Request request;
    try {
      request.AppendRawData(requests["head"]);
      FAIL();
    } catch (Request::RequestFatalException &e) {
      EXPECT_STREQ(e.what(), "Invalid method");
    }
  }
}

TEST_F(RequestTest, Invalid_request) {
  {
    Request request;
    try {
      request.AppendRawData(requests["invalid"]);
      FAIL();
    } catch (Request::RequestFatalException &e) {
      EXPECT_STREQ(e.what(), "Invalid startline");
    }
  }
  {
    Request request;
    try {
      request.AppendRawData(requests["no length"]);
      FAIL();
    } catch (Request::RequestFatalException &e) {
      EXPECT_STREQ(e.what(), "Length Required");
    }
  }
}

TEST_F(RequestTest, Body) {
  {
    Request request;
    request.AppendRawData(requests["chunk"]);
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Content-Type"), "text/plain");
    EXPECT_EQ(request.GetHeader("Transfer-Encoding"), "chunked");
    EXPECT_EQ(request.GetBody(), "MozillaDeveloperNetwork");
  }
  {
    Request request;
    request.AppendRawData(requests["body"]);
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
    EXPECT_EQ(request.GetBody(), "hello world. this isa test.");
  }
}