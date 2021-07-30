#include "Socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

#include <stdexcept>

void Socket::SetStatus(enum SocketStatus status) { socket_status_ = status; }
