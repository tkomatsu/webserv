#include <gtest/gtest.h>

#include "Server.hpp"

class ServerTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}

  void CheckStatus(int a, int b) { EXPECT_EQ(a, b); }
};

TEST_F(ServerTest, SetStatus) {
  Server server(4200, "127.0.0.1");

  server.SetStatus(WRITE_CLIENT);
  CheckStatus(WRITE_CLIENT, server.GetStatus());
}

TEST_F(ServerTest, SetSocket) {
  Server server(4200, "127.0.0.1");

  int fd = server.SetSocket(42);
  EXPECT_GT(fd, 0);
}
