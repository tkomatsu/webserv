#include <gtest/gtest.h>
#include <map>
#include "Request.hpp"

class RequestTest : public testing::Test {
protected:
  virtual void SetUp() {
    requests["get"] = "GET /index.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.example.com\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\n";
    requests["post1"] = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 49\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\nlicenseID=string&content=string&/paramsXML=string";
    requests["post2"] = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.example.com\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: 90\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\n<?xml version=\"1.\" encoding\"utf-\"?>\r\n<string xmlns\"http://clearforest.com\">string</string>";
    requests["delete"] = "DELETE / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["put"] = "PUT / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["head"] = "HEAD / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["invalid"] = "Host:www.google.com\r\n\r\n";
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
    EXPECT_EQ(request.GetURI(), "/index.html");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.example.com");
    EXPECT_EQ(request.GetHeader("User-Agent"), "Mozilla/4.0 (compatible; MSIE5.01; Windows NT)");
    EXPECT_EQ(request.GetHeader("Accept-Language"), "en-us");
    EXPECT_EQ(request.GetHeader("Accept-Encoding"), "gzip, deflate");
    EXPECT_EQ(request.GetHeader("Connection"), "Keep-Alive");
    EXPECT_EQ(request.GetBody() ,"");
  }
  {
    Request request;
    request.AppendRawData(requests["post1"]);
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/cgi-bin/process.cgi");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.example.com");
    EXPECT_EQ(request.GetHeader("User-Agent"), "Mozilla/4.0 (compatible; MSIE5.01; Windows NT)");
    EXPECT_EQ(request.GetHeader("Content-Type"), "application/x-www-form-urlencoded");
    EXPECT_EQ(request.GetHeader("Content-Length"), "49");
    EXPECT_EQ(request.GetHeader("Accept-Language"), "en-us");
    EXPECT_EQ(request.GetHeader("Accept-Encoding"), "gzip, deflate");
    EXPECT_EQ(request.GetHeader("Connection"), "Keep-Alive");
    EXPECT_EQ(request.GetBody() ,"licenseID=string&content=string&/paramsXML=string");
  }
  {
    Request request;
    request.AppendRawData(requests["post2"]);
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/cgi-bin/process.cgi");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.example.com");
    EXPECT_EQ(request.GetHeader("User-Agent"), "Mozilla/4.0 (compatible; MSIE5.01; Windows NT)");
    EXPECT_EQ(request.GetHeader("Content-Type"), "text/xml; charset=utf-8");
    EXPECT_EQ(request.GetHeader("Content-Length"), "90");
    EXPECT_EQ(request.GetHeader("Accept-Language"), "en-us");
    EXPECT_EQ(request.GetHeader("Accept-Encoding"), "gzip, deflate");
    EXPECT_EQ(request.GetHeader("Connection"), "Keep-Alive");
    EXPECT_EQ(request.GetBody() ,"<?xml version=\"1.\" encoding\"utf-\"?>\r\n<string xmlns\"http://clearforest.com\">string</string>");
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
