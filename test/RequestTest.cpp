#include <gtest/gtest.h>

#include <map>

#include "Request.hpp"

class RequestTest : public testing::Test {
 protected:
  virtual void SetUp() {
    std::string s =
        "GET /index.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; "
        "MSIE5.01; Windows NT)\r\nHost: www.example.com\r\nAccept-Language: "
        "en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: "
        "Keep-Alive\r\n\r\n";
    requests["get"] = s.c_str();
    s = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 "
        "(compatible; MSIE5.01; Windows NT)\r\nHost: "
        "www.example.com\r\nContent-Type: "
        "application/x-www-form-urlencoded\r\nContent-Length: "
        "49\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, "
        "deflate\r\nConnection: "
        "Keep-Alive\r\n\r\nlicenseID=string&content=string&/paramsXML=string";
    requests["post1"] = s.c_str();
    s = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 "
        "(compatible; MSIE5.01; Windows NT)\r\nHost: "
        "www.example.com\r\nContent-Type: text/xml; "
        "charset=utf-8\r\nContent-Length: 90\r\nAccept-Language: "
        "en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: "
        "Keep-Alive\r\n\r\n<?xml version=\"1.\" encoding\"utf-\"?>\r\n<string "
        "xmlns\"http://clearforest.com\">string</string>";
    requests["post2"] = s.c_str();
    s = "DELETE / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["delete"] = s.c_str();
    s = "PUT / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["put"] = s.c_str();
    s = "HEAD / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["head"] = s.c_str();
    s = "Host:www.google.com\r\n\r\n";
    requests["invalid"] = s.c_str();
    s = "POST / HTTP/1.1\r\nHost: www.google.com\r\nContent-Length: "
        "27\r\n\r\nhello world. this isa test.";
    requests["body"] = s.c_str();
    s = "POST / HTTP/1.1\r\nContent-Type: text/plain\r\nTransfer-Encoding: "
        "chunked\r\n\r\n7\r\nMozilla\r\n9\r\nDeveloper\r\n7\r\nNetwork\r\n0\r\n"
        "\r\n";
    requests["chunk"] = s.c_str();
  }
  virtual void TearDown() {}

  std::map<std::string, const char *> requests;
};

/*
TEST_F(RequestTest, Standard) {
  {
    Request request;
    request.AppendRawData(requests["get"], strlen(requests["get"]));
    EXPECT_EQ(request.GetMethod(), GET);
    EXPECT_EQ(request.GetURI(), "/index.html");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.example.com");
    EXPECT_EQ(request.GetHeader("User-Agent"),
              "Mozilla/4.0 (compatible; MSIE5.01; Windows NT)");
    EXPECT_EQ(request.GetHeader("Accept-Language"), "en-us");
    EXPECT_EQ(request.GetHeader("Accept-Encoding"), "gzip, deflate");
    EXPECT_EQ(request.GetHeader("Connection"), "Keep-Alive");
    std::string test_body = "";
    std::vector<unsigned char> v(test_body.begin(), test_body.end());
    EXPECT_EQ(request.GetBody(), v);
  }

  {
    Request request;
    request.AppendRawData(requests["post1"], strlen(requests["post1"]));
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/cgi-bin/process.cgi");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.example.com");
    EXPECT_EQ(request.GetHeader("User-Agent"),
              "Mozilla/4.0 (compatible; MSIE5.01; Windows NT)");
    EXPECT_EQ(request.GetHeader("Content-Type"),
              "application/x-www-form-urlencoded");
    EXPECT_EQ(request.GetHeader("Content-Length"), "49");
    EXPECT_EQ(request.GetHeader("Accept-Language"), "en-us");
    EXPECT_EQ(request.GetHeader("Accept-Encoding"), "gzip, deflate");
    EXPECT_EQ(request.GetHeader("Connection"), "Keep-Alive");
    std::string test_body = "licenseID=string&content=string&/paramsXML=string";
    std::vector<unsigned char> v(test_body.begin(), test_body.end());
    EXPECT_EQ(request.GetBody(), v);
  }

  {
    Request request;
    request.AppendRawData(requests["post2"], strlen(requests["post2"]));
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/cgi-bin/process.cgi");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.example.com");
    EXPECT_EQ(request.GetHeader("User-Agent"),
              "Mozilla/4.0 (compatible; MSIE5.01; Windows NT)");
    EXPECT_EQ(request.GetHeader("Content-Type"), "text/xml; charset=utf-8");
    EXPECT_EQ(request.GetHeader("Content-Length"), "90");
    EXPECT_EQ(request.GetHeader("Accept-Language"), "en-us");
    EXPECT_EQ(request.GetHeader("Accept-Encoding"), "gzip, deflate");
    EXPECT_EQ(request.GetHeader("Connection"), "Keep-Alive");
    std::string test_body =
        "<?xml version=\"1.\" encoding\"utf-\"?>\r\n<string "
        "xmlns\"http://clearforest.com\">string</string>";
    std::vector<unsigned char> v(test_body.begin(), test_body.end());
    EXPECT_EQ(request.GetBody(), v);
  }

  {
    Request request;
    request.AppendRawData(requests["delete"], strlen(requests["delete"]));
    EXPECT_EQ(request.GetMethod(), DELETE);
    EXPECT_EQ(request.GetURI(), "/");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
    std::string test_body = "";
    std::vector<unsigned char> v(test_body.begin(), test_body.end());
    EXPECT_EQ(request.GetBody(), v);
  }
}

TEST_F(RequestTest, Invalid_method) {
  {
    Request request;
    try {
      request.AppendRawData(requests["put"], strlen(requests["put"]));
      FAIL();
    } catch (Request::RequestFatalException &e) {
      EXPECT_STREQ(e.what(), "Invalid method");
    }
  }
  {
    Request request;
    try {
      request.AppendRawData(requests["head"], strlen(requests["head"]));
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
      request.AppendRawData(requests["invalid"], strlen(requests["invalid"]));
      FAIL();
    } catch (Request::RequestFatalException &e) {
      EXPECT_STREQ(e.what(), "Invalid startline");
    }
  }
}

TEST_F(RequestTest, Body) {
  {
    Request request;
    request.AppendRawData(requests["chunk"], strlen(requests["chunk"]));
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Content-Type"), "text/plain");
    EXPECT_EQ(request.GetHeader("Transfer-Encoding"), "chunked");
    std::string test_body = "MozillaDeveloperNetwork";
    std::vector<unsigned char> v(test_body.begin(), test_body.end());
    EXPECT_EQ(request.GetBody(), v);
  }

  {
    Request request;
    request.AppendRawData(requests["body"], strlen(requests["body"]));
    EXPECT_EQ(request.GetMethod(), POST);
    EXPECT_EQ(request.GetURI(), "/");
    EXPECT_EQ(request.GetVersion(), "1.1");
    EXPECT_EQ(request.GetHeader("Host"), "www.google.com");
    std::string test_body = "hello world. this isa test.";
    std::vector<unsigned char> v(test_body.begin(), test_body.end());
    EXPECT_EQ(request.GetBody(), v);
  }
}
*/
