/* Start Header
*****************************************************************/
/*!
\file SocketUtility.h
\author Low Jin Liang Aaron, aaron.low, 390001116
\par aaron.low\@digipen.edu
\date 12/11/2019

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#ifndef _SOCKET_UTILITY_H_
#define _SOCKET_UTILITY_H_

#define PRINTDEBUG 0

#include "UDPSocket.h"
#include "TCPSocket.h"
#include <vector>
#include <iostream>

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
  static fd_set* FillSetFromVector(fd_set& outSet, const std::vector<TCPSocketPtr>* inSockets, int* nfds = nullptr);
  static void FillVectorFromSet(std::vector<TCPSocketPtr>* outSockets, const std::vector<TCPSocketPtr>* inSockets, const fd_set& inSet);
  static int Select(const std::vector<TCPSocketPtr>* inReadSet, std::vector<TCPSocketPtr>* outReadSet,
                    const std::vector<TCPSocketPtr>* inWriteSet, std::vector<TCPSocketPtr>* outWriteSet,
                    const std::vector<TCPSocketPtr>* inExcepSet, std::vector<TCPSocketPtr>* outExcepSet,
                    timeval* timeout = nullptr);
  static fd_set* FillSetFromVector(fd_set& outSet, const std::vector<UDPSocketPtr>* inSockets, int* nfds = nullptr);
  static void FillVectorFromSet(std::vector<UDPSocketPtr>* outSockets, const std::vector<UDPSocketPtr>* inSockets, const fd_set& inSet);
  static int Select(const std::vector<UDPSocketPtr>* inReadSet, std::vector<UDPSocketPtr>* outReadSet,
                    const std::vector<UDPSocketPtr>* inWriteSet, std::vector<UDPSocketPtr>* outWriteSet,
                    const std::vector<UDPSocketPtr>* inExcepSet, std::vector<UDPSocketPtr>* outExcepSet,
                    timeval* timeout = nullptr);
};

#endif
