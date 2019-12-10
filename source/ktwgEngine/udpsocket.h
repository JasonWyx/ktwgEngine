/* Start Header
*****************************************************************/
/*!
\file UDPSocket.h
\author Low Jin Liang Aaron, aaron.low, 390001116
\par aaron.low\@digipen.edu
\date 12/11/2019

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#ifndef _UDP_SOCKET_H_
#define _UDP_SOCKET_H_

#include "Sockets.h"
#include <memory>

class SocketAddress;

class UDPSocket
{
  friend class SocketUtility;

  SOCKET socket_;

  UDPSocket(SOCKET inSocket);
public:
  ~UDPSocket();

  int Bind(const SocketAddress& inDestinationAddress);
  int SendTo(const void* inData, int inLen, const SocketAddress& inDestination);
  int ReceiveFrom(void* inBuffer, int inLen, SocketAddress& outFrom);
  int SetBlockingMode(u_long blocking);
};

using UDPSocketPtr = std::shared_ptr<UDPSocket>;

#endif
