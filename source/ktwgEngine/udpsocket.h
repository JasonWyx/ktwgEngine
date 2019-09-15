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
