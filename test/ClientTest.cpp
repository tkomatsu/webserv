#include <gtest/gtest.h>

#include <regex>
#include <string>

#include "Client.hpp"
#include "Server.hpp"
#include "config.hpp"

class ClientTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

/*
TEST_F(ClientTest, IsValidExtension) {
  config::Parser parser("../../conf/default.conf");
  std::vector<config::Config> configs = parser.GetConfigs();

  Server server(configs[0]);  // default.conf's first server(listening 4200)
  Client client(server.GetConfig());

  EXPECT_TRUE(client.IsValidExtension("./docs/abc.py", "/abc.py"));
  EXPECT_TRUE(client.IsValidExtension("./docs/abc.php", "/abc.php"));
  EXPECT_TRUE(client.IsValidExtension("./docs/z.perl.php.cgi.py",
                                      "/z.perl.php.cgi.py"));
  EXPECT_TRUE(client.IsValidExtension("./docs/abc.py", "/abc.py/"));
  EXPECT_TRUE(client.IsValidExtension("./docs/abc.php", "/abc.php/"));
  EXPECT_TRUE(client.IsValidExtension("./docs/z.perl.php.cgi.py",
                                      "/z.perl.php.cgi.py/"));
  EXPECT_FALSE(client.IsValidExtension("./docs/abc.cgi", "/abc.cgi/"));
  EXPECT_FALSE(client.IsValidExtension("./docs/abc.cgi", "/abc.cgi"));
  EXPECT_FALSE(client.IsValidExtension("./docs/", "/"));
  EXPECT_FALSE(client.IsValidExtension("./docs/upload", "/upload"));
  EXPECT_FALSE(client.IsValidExtension("./docs/upload", "/upload/"));
}

TEST_F(ClientTest, GetIndexFileIfExist) {
  config::Parser parser("../../conf/default.conf");
  std::vector<config::Config> configs = parser.GetConfigs();

  Server server(configs[0]);
  Client client(server.GetConfig());

  EXPECT_EQ(client.GetIndexFileIfExist("../../docs/cgi-bin/", "/cgi/cgi-bin/"),
"index.php"); EXPECT_EQ(client.GetIndexFileIfExist("../../docs/cgi-bin/",
"/cgi/cgi-bin"), "index.php");
  EXPECT_EQ(client.GetIndexFileIfExist("../../docs/html/", "/"), "index.html");
  system("mv ../../docs/html/index.html ../../docs/html/index.htm");
  EXPECT_EQ(client.GetIndexFileIfExist("../../docs/html/", "/"), "index.htm");
  system("mv ../../docs/html/index.htm ../../docs/html/index.html");
}

TEST_F(ClientTest, IsValidUploadRequest) {
  config::Parser parser("../../conf/default.conf");
  std::vector<config::Config> configs = parser.GetConfigs();

  Server server(configs[1]);  // default.conf's second server(listening 4201)
  Client client(server.GetConfig());

  chdir("../../");
  EXPECT_TRUE(client.IsValidUploadRequest("/upload"));
  EXPECT_TRUE(client.IsValidUploadRequest("/upload"));
  EXPECT_TRUE(client.IsValidUploadRequest("/upload/"));
  EXPECT_TRUE(client.IsValidUploadRequest("/upload/"));
  EXPECT_TRUE(client.IsValidUploadRequest("/upload/new.html"));
  EXPECT_TRUE(client.IsValidUploadRequest("/upload/new.html/"));
  EXPECT_FALSE(client.IsValidUploadRequest("/"));
  EXPECT_FALSE(client.IsValidUploadRequest("/hoge"));
  EXPECT_FALSE(client.IsValidUploadRequest("/hoge/upload"));
  EXPECT_FALSE(client.IsValidUploadRequest("/hoge/upload/"));
  EXPECT_FALSE(client.IsValidUploadRequest("/hoge/upload/new.html"));
  EXPECT_FALSE(client.IsValidUploadRequest("/hoge/upload/new.html"));
}

TEST_F(ClientTest, MakePathUri) {
  config::Parser parser("../../conf/default.conf");
  std::vector<config::Config> configs = parser.GetConfigs();

  Server server(configs[1]);  // default.conf's second server(listening 4201)
  Client client(server.GetConfig());

  // std::string MakePathUri(std::string alias_path, std::string request_uri,
  // std::string location_path);
  // alias_path + (request_uri - location_path)
  EXPECT_EQ(client.MakePathUri("/i/top.gif", "/i/"), "./docs/top.gif");
  EXPECT_EQ(client.MakePathUri("/i/top.gif", "/i/"), "./docs/top.gif");
  EXPECT_EQ(client.MakePathUri("/i/top.gif", "/"), "./docs/i/top.gif");
  EXPECT_EQ(client.MakePathUri("/i/top.gif", "/i/"), "./docs/top.gif");
}
*/
