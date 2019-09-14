#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include "Sockets.h"
#include <memory>

class SocketAddress;

class TCPSocket
{
  friend class SocketUtility;

  SOCKET socket_;

  TCPSocket(SOCKET inSocket);

public:
  ~TCPSocket();
  int Connect(const SocketAddress& inAddress);
  int Bind(const SocketAddress& inAddress);
  int Listen(int inBackLog = 32);
  std::shared_ptr<TCPSocket> Accept(SocketAddress& outFromAddress);
  int Send(const void* inData, int inLen);
  int Receive(void* inBuffer, int inLen);
  int Shutdown(int inHow);
  int SetNonBlocking(bool inBlocking);
  int SetSockOpt(int inOptionName, const char* inOptionValue, socklen_t inOptionLen);
};

using TCPSocketPtr = std::shared_ptr<TCPSocket>;
#endif
