#include "Server.hpp"

void Server::init(void) {
  // サーバー側のlistenするためのfdを得る
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    throw std::runtime_error("socket error\n");

  // bindのための設定。ipとかポートを割り当てる
  memset((char *)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(host);
  addr.sin_port = htons(port);

  // ソケットをノンブロッキングにさせる(今無理〜ってときに即エラー返す)
  if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
    throw std::runtime_error("fcntl error\n");

  // TIME_WAIT問題を解決する。やってもやんなくてもよさそう。
  int on = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    throw std::runtime_error("bind error\n");

  // appachは500程度らしいから512にしといた　http://www.coins.tsukuba.ac.jp/~syspro/2018/2018-05-30/echo-server-nofork-fdopen.html#tcp_acc_port
  if (listen(fd, 512) == -1) throw std::runtime_error("listen error\n");
}