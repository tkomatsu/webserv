#include <gtest/gtest.h>

#include "Server.hpp"

class ServerTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(ServerTest, SetStatus) { Server server(4200, "127.0.0.1"); }

TEST_F(ServerTest, SetSocket) {
  Server server(4200, "127.0.0.1");

  int fd = server.SetSocket();
  EXPECT_GT(fd, 0);
}
