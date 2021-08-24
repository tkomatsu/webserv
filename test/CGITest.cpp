#include <gtest/gtest.h>

#include "CGI.hpp"
#include "config.hpp"

class CGITest : public testing::Test {
 protected:
  std::map<std::string, std::string> requests;

  virtual void SetUp() {
    requests["get"] =
        "GET /index.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; "
        "MSIE5.01; Windows NT)\r\nHost: www.example.com\r\nAccept-Language: "
        "en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: "
        "Keep-Alive\r\n\r\n";
    requests["post1"] =
        "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 "
        "(compatible; MSIE5.01; Windows NT)\r\nHost: "
        "www.example.com\r\nContent-Type: "
        "application/x-www-form-urlencoded\r\nContent-Length: "
        "49\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, "
        "deflate\r\nConnection: "
        "Keep-Alive\r\n\r\nlicenseID=string&content=string&/paramsXML=string";
    requests["post2"] =
        "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 "
        "(compatible; MSIE5.01; Windows NT)\r\nHost: "
        "www.example.com\r\nContent-Type: text/xml; "
        "charset=utf-8\r\nContent-Length: 90\r\nAccept-Language: "
        "en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: "
        "Keep-Alive\r\n\r\n<?xml version=\"1.\" encoding\"utf-\"?>\r\n<string "
        "xmlns\"http://clearforest.com\">string</string>";
    requests["delete"] = "DELETE / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["put"] = "PUT / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["head"] = "HEAD / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    requests["invalid"] = "Host:www.google.com\r\n\r\n";
    requests["body"] =
        "POST / HTTP/1.1\r\nHost: www.google.com\r\nContent-Length: "
        "27\r\n\r\nhello world. this isa test.";
    requests["chunk"] =
        "POST / HTTP/1.1\r\nContent-Type: text/plain\r\nTransfer-Encoding: "
        "chunked\r\n\r\n7\r\nMozilla\r\n9\r\nDeveloper\r\n7\r\nNetwork\r\n0\r\n"
        "\r\n";
    // config.port = 4200;
  }
  virtual void TearDown() {}

  void ExpectArgs(CGI& res) {
    EXPECT_STREQ(res.GetArgs()[0], "./docs/perl.cgi");
    EXPECT_STREQ(res.GetArgs()[1], "mcgee");
    EXPECT_STREQ(res.GetArgs()[2], "mine");
  }

  void ExpectEnvs(CGI& res) {
    EXPECT_STREQ(res.GetEnvs()[0], "AUTH_TYPE=");
    EXPECT_STREQ(res.GetEnvs()[1], "CONTENT_LENGTH=");
    EXPECT_STREQ(res.GetEnvs()[2], "CONTENT_TYPE=");
  }
};

TEST_F(CGITest, Args) {
  Request req;
  req.AppendRawData(requests["get"]);
  config::Main main_context;
  config::Server server_context(main_context);
  server_context.port = 4200;
  server_context.host = "127.0.0.1";
  config::Config config(server_context);
  CGI cgi(req, 42, "a", config);

  ExpectArgs(cgi);
}

TEST_F(CGITest, Envs) {
  Request req;
  req.AppendRawData(requests["get"]);
  config::Main main_context;
  config::Server server_context(main_context);
  server_context.port = 4200;
  server_context.host = "127.0.0.1";
  config::Config config(server_context);
  CGI cgi(req, 42, "a", config);

  ExpectArgs(cgi);
}
