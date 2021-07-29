#include <gtest/gtest.h>

#include "Client.hpp"
#include "Server.hpp"

class ClientTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}

  /* void CheckGenFd(int fd1, int fd2) {
   EXPECT_NE(fd1, fd2);
 } */
};

// accept error回避したいけど…
TEST_F(ClientTest, SetSocket) {
  Server server(4200, "127.0.0.1");
  Client client;
  int tmp_fd = server.SetSocket(42);

  try {
    int res = client.SetSocket(tmp_fd);
  } catch (std::runtime_error const& err) {
    EXPECT_EQ(err.what(), std::string("accept error\n"));
  }
}

// うまく動かない。関数が呼ばれてない
TEST_F(ClientTest, AutoIndex) {
  Client client;	  std::cout << "きた" << std::endl;
							// docsフォルダのフルパス
  std::string res = client.MakeAutoIndexContent("/Users/yudai/webserv/docs");

  std::cout << res << std::endl;
}
