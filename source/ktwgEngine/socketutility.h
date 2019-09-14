#ifndef _SOCKET_UTILITY_H_
#define _SOCKET_UTILITY_H_

#define PRINTDEBUG 0

#include "UDPSocket.h"
#include "TCPSocket.h"

enum SocketAddressFamily
{
  IPv4 = AF_INET,
  IPv6 = AF_INET6
};

class SocketUtility
{

public:
  static bool Init();
  static void CleanUp();

  static void ReportError(const char* inOpDesc);
  static int GetLastError();

  static UDPSocketPtr CreateUDPSocket(SocketAddressFamily inFamily);
  static TCPSocketPtr CreateTCPSocket(SocketAddressFamily inFamily);
};

#endif
