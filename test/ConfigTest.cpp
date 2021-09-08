#include <gtest/gtest.h>

#include "config.hpp"

class LineBuilderTest : public testing::Test {
  protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST(LineBuilderTest, TestEmpty) {
  std::ifstream file("../conf/empty.conf");
  config::LineBuilder builder(file);
  config::LineComponent line;

  EXPECT_FALSE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::NONE);
}

TEST(LineBuilderTest, TestWhitespace) {
  std::ifstream file("../conf/whitespaces.conf");
  config::LineBuilder builder(file);
  config::LineComponent line;

  EXPECT_FALSE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::NONE);
}

TEST(LineBuilderTest, TestComments) {
  std::ifstream file("../conf/comments.conf");
  config::LineBuilder builder(file);
  config::LineComponent line;

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "print_config");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "on");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "index");
  EXPECT_EQ(line.params.size(), 3);
  EXPECT_EQ(line.params[0], "index.html#");
  EXPECT_EQ(line.params[1], "autoindex");
  EXPECT_EQ(line.params[2], "on");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "listen");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "8000");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server_name");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "server1.com");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "location");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "/");
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "alias");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "/var/www/html/");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "allowed_methods");
  EXPECT_EQ(line.params.size(), 3);
  EXPECT_EQ(line.params[0], "GET");
  EXPECT_EQ(line.params[1], "XXX");
  EXPECT_EQ(line.params[2], "YYY");
  EXPECT_EQ(line.type, config::SIMPLE);
  
  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_FALSE(builder.GetNext(line));
}

TEST(LineBuilderTest, TestIndents) {
  std::ifstream file("../conf/indents.conf");
  config::LineBuilder builder(file);
  config::LineComponent line;

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "what");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "what");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "what");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "what");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server"); 
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "index");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "what.html#");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, ""); 
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server"); 
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "listen");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "8001");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "root");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "/var/www/html");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "location");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "/test/");
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "index");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "index.html");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "location");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "/images/");
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "index");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "none.html");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "listen");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "8002");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "listen");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "8003");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "index");
  EXPECT_EQ(line.params.size(), 3);
  EXPECT_EQ(line.params[0], "index.nginx-debian.html");
  EXPECT_EQ(line.params[1], "autoindex");
  EXPECT_EQ(line.params[2], "on");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "index");
  EXPECT_EQ(line.params.size(), 4);
  EXPECT_EQ(line.params[0], "autoindex");
  EXPECT_EQ(line.params[1], "on");
  EXPECT_EQ(line.params[2], "index");
  EXPECT_EQ(line.params[3], "nginx-debian.html");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_START);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "listen");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "8004");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "server_name");
  EXPECT_EQ(line.params.size(), 1);
  EXPECT_EQ(line.params[0], "test2");
  EXPECT_EQ(line.type, config::SIMPLE);

  EXPECT_TRUE(builder.GetNext(line));
  EXPECT_EQ(line.name, "");
  EXPECT_EQ(line.params.size(), 0);
  EXPECT_EQ(line.type, config::BLOCK_END);

  EXPECT_FALSE(builder.GetNext(line));
}
