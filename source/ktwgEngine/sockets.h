#ifndef _SOCKETS_H_
#define _SOCKETS_H_

#if _WIN32

#include "win32_includes.h"

#pragma comment(lib, "ws2_32.lib")

#else


#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>

using SOCKET = int; // typedef for base socket type
using ADDRESS_FAMILY = uint16_t;

#define UNREFERENCED_PARAMETER(p) (void)(p)
#define SOCKET_ERROR -1
#define NO_ERROR 0
#define INVALID_SOCKET -1

#endif

#if _WIN32

static constexpr int WOULDBLOCK = -WSAEWOULDBLOCK;
static constexpr int ISCONN = -WSAEISCONN;
static constexpr int SEND = SD_SEND;

#else

static constexpr int WOULDBLOCK = -EINPROGRESS;
static constexpr int ISCONN = -EISCONN;
static constexpr int SEND = SHUT_WR;
#define TRUE 1

#endif

#endif
