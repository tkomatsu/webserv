#include "WebServ.hpp"

const std::string WebServ::default_path = "./default.conf";
const int WebServ::buf_max = 8192;

void WebServ::parse(const std::string &path)
{
	// TODO: configをパースして各サーバーの設定をServerインスタンスにもたせる
	// (今は最低限のホストアドレスとポートしかない)
	(void)path;

	// ホスト127.0.0.1、ポート4200, 4201, 4202のソケットを
	// とりま今は直打ちで生成
	for (int i = 0; i < 3; ++i)
	{
		Server serv(4200 + i, "127.0.0.1");

		serv.init();
		servers.push_back(serv);
	}
}

void WebServ::start(void)
{
	max_fd = 0;
	FD_ZERO(&master_set);

	// サーバーのソケット（ファイルディスクリプタ）をmaster_setに登録
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		Server serv = *it;
		long serv_fd = serv.getFd();

		FD_SET(serv_fd, &master_set);
		if (serv_fd > max_fd)
			max_fd = serv_fd;
	}
	if (max_fd == 0)
		throw std::runtime_error( "no server error\n");

	while (true)
	{
			fd_set rfd_set;
			fd_set wfd_set;
			struct timeval timeout = (struct timeval){1, 0};
			int n = 0;

			// readable/writableが見つかるまで待つ
			while (n == 0)
			{
				memcpy(&rfd_set, &master_set, sizeof(master_set));
				FD_ZERO(&wfd_set);
				for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
					for (std::vector<long>::iterator it_ = (*it).writable_client_fds.begin(); it_ != (*it).writable_client_fds.end(); ++it_)
						FD_SET(*it_, &wfd_set);

				std::cout << "😺😺 selecting 😺😺\n";
				n = select(max_fd + 1, &rfd_set, &wfd_set, NULL, &timeout);
			}

			if (n > 0)
			{
				// 各サーバーの"クライアント"のソケットを見る
				// writableのやつが来たら
				for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
				{
					Server &serv = *it;

					for (std::vector<long>::iterator it = serv.writable_client_fds.begin(); it != serv.writable_client_fds.end(); ++it)
					{
						long client_fd = *it;

						if (FD_ISSET(client_fd, &wfd_set))
						{
							// TODO:　送る内容しっかり作ろう
							long ret;
							std::string header;
							std::string body;
							std::string response;
							std::ifstream ifs("index.html");
							std::string line;
							// とりま直打ち
							header = "HTTP/1.1 200 OK\r\n"
									 "Content-Type: text/html; charset=UTF-8\r\n"
									 "Date: Wed, 30 Jun 2021 08:25:23 GMT\r\n"
									 "Server: Webserv\r\n"
									 "\r\n";

							if (ifs.fail())
								throw std::runtime_error( "file open error\n");
							while (getline(ifs, line))
								body += line + "\n";
							ifs.close();
							response = header + body;
							ret = send(client_fd, response.c_str(), response.size(), 0);
						
							std::cout << "\n\x1b[36m██████████ ここから　レスポンス ██████████\n";
							std::cout << response;
							std::cout << "██████████ ここまで　レスポンス ██████████\x1b[39m\n\n";
							// TODO: 一回のsendで送りきれないときどっかにためとく?　そんなことある？

							close(client_fd);
							FD_CLR(client_fd, &rfd_set);
							FD_CLR(client_fd, &master_set);
							serv.writable_client_fds.erase(it);
														
							n = 0;
							break;
						}
					}
					if (n == 0)
						break;
				}

				// 各サーバーの"クライアント"のソケットを見る
				// readableの奴が来たら読み取って、もし最後(\r\n\r\n)まで読んだら書き出す & writableの候補に追加
				if (n > 0)
					for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
					{
						Server &serv = *it;

						for (std::vector<long>::iterator it = serv.client_fds.begin(); it != serv.client_fds.end(); ++it)
						{
							long client_fd = *it;

							if (FD_ISSET(client_fd, &rfd_set))
							{
								char buf[WebServ::buf_max] = {0};
								long ret;

								ret = recv(client_fd, buf, WebServ::buf_max - 1, 0);
								if (ret == -1)
								{
									close(client_fd);
									FD_CLR(client_fd, &rfd_set);
									FD_CLR(client_fd, &master_set);
									serv.client_fds.erase(it);
									throw std::runtime_error( "recv error\n");
								}
								else if (ret == 0)
								{
									// clientがcurlして応答待ちしてるときにCtrl + Cしたらここに来る
									close(client_fd);
									FD_CLR(client_fd, &rfd_set);
									FD_CLR(client_fd, &master_set);
									serv.client_fds.erase(it);
								}
								else
								{
									size_t i = std::string(buf).find("\r\n\r\n");
									if (i != std::string::npos)
									{
										std::cout << "\n\x1b[35m██████████ ここから　リクエスト ██████████\n";
										std::cout << buf;
										std::cout << "██████████ ここまで　リクエスト ██████████\x1b[39m\n\n";
										serv.writable_client_fds.push_back(client_fd);
									}
									// TODO: 一回のrecvで取りきれないときどっかにためとく?
								}
								n = 0;
								break;
							}
						}
						if (n == 0)
							break;
					}

				// 各サーバーのソケットを見る
				// 新たなクライアント様はこちらでaccept致します
				if (n > 0)
					for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
					{
						Server &serv = *it;
						long serv_fd = serv.getFd();

						if (FD_ISSET(serv_fd, &rfd_set))
						{
							long client_fd = accept(serv_fd, NULL, NULL);

							if (client_fd == -1)
								throw std::runtime_error( "accept error\n");
							if (fcntl(client_fd, F_SETFL, O_NONBLOCK) != 0)
								throw std::runtime_error( "fcntl error\n");
						
							FD_SET(client_fd, &master_set);
							if (client_fd > max_fd)
								max_fd = client_fd;
							serv.client_fds.push_back(client_fd);
							n = 0;
							break;
						}
					}
			}
			else
			{
				throw std::runtime_error( "select error\n");
			}
	}
}
