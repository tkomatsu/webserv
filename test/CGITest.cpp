#include <gtest/gtest.h>

#include "CGI.hpp"

/* #include "gmock.h"
using ::testing::MatchesRegex; */

class CGITest : public testing::Test {
 protected:
  virtual void SetUp() {}
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
  CGI cgi(req);

  ExpectArgs(cgi);
}

TEST_F(CGITest, Envs) {
  Request req;
  CGI cgi(req);

  ExpectArgs(cgi);
}
