#ifndef _SOCKET_ADDRESS_H_
#define _SOCKET_ADDRESS_H_

#include "Sockets.h"
#include <memory>
#include <cinttypes>

class SocketAddress
{
  friend class UDPSocket;
  friend class TCPSocket;

  sockaddr sockAddress_; // The socket address

public:
  SocketAddress(int family, uint32_t address, uint16_t port);
  SocketAddress(const sockaddr& sockAddr);
  
  sockaddr_in* GetAsSockAddrIn();

  inline size_t GetSize() const
  {
    return sizeof(sockAddress_);
  }
};

using SocketAddressPtr = std::shared_ptr<SocketAddress>;

#endif
