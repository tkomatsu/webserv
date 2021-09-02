#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include "CGI.hpp"
#include "ISocket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "config.hpp"
#include "utility.hpp"

#ifdef GOOGLE_TEST
#include <gtest/gtest.h>
#endif

class Client : public ISocket {
#ifdef GOOGLE_TEST
  FRIEND_TEST(ClientTest, IsValidExtension);
  FRIEND_TEST(ClientTest, GetIndexFileIfExist);
  FRIEND_TEST(ClientTest, IsValidUploadRequest);
  FRIEND_TEST(ClientTest, MakePathUri);
#endif

 public:
  static const int buf_max_;
  Client(const config::Config &config);

  // Socket actions
  int ConnectClientSocket(int _fd);

  // I/O actions
  int RecvRequest(int client_fd);
  int ReadStaticFile();
  int ReadCGIout();
  int SendResponse(int client_fd);
  void WriteStaticFile();
  void WriteCGIin();

  // Getter
  int GetStatus() const;
  int GetWriteFd() const;
  int GetReadFd() const;

  // Exception handling
  void HandleException(const char *err_msg);

 private:
  /* prohibit copy constructor and assignment operator */
  Client(const Client &client);
  Client &operator=(const Client &client);

  void Preprocess(void);
  void GenProcessForCGI(const std::string &path_uri);
  void SetPipe(int *pipe_write, int *pipe_read);
  void ExecCGI(int *pipe_write, int *pipe_read, const CGI &cgi,
               const std::string &path_uri);

  bool IsValidExtension(std::string path_uri, std::string request_path);
  bool IsValidUploadRequest(const std::string &request_path);
  bool DirectoryRedirect(std::string request_path);
  std::string GetIndexFileIfExist(std::string path_uri,
                                  std::string request_path);
  enum SocketStatus GetNextOfReadClient(std::string &path_uri);
  std::string MakePathUri(std::string request_uri, std::string location_path);

  // method branching
  enum SocketStatus HandleGET(std::string &path_uri);
  enum SocketStatus HandlePOST(std::string &path_uri);
  enum SocketStatus HandleDELETE(std::string &path_uri);

  // logging
  std::string LogMessage() const;

  // member variables
  Request request_;
  Response response_;
  size_t sended_;  // num of chars sended to client
  int write_fd_;
  int read_fd_;
  const config::Config config_;
};

#endif /* CLIENT_HPP */
