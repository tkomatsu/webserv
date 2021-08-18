#include <gtest/gtest.h>

#include "Server.hpp"
#include "config.hpp"

class ServerTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(ServerTest, SetStatus) {
  config::Main main_context;
  config::Server server_context(0, main_context);
  server_context.port = 4200;
  server_context.host = "127.0.0.1";

  config::Config config(server_context);

  Server server(config);
}

TEST_F(ServerTest, SetSocket) {
  config::Main main_context;
  config::Server server_context(0, main_context);
  server_context.port = 4200;
  server_context.host = "127.0.0.1";

  config::Config config(server_context);
  Server server(config);

  int fd = server.SetSocket();
  EXPECT_GT(fd, 0);
}
