#ifndef _SOCKETS_H_
#define _SOCKETS_H_

#if _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

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

#endif
