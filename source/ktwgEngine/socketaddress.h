/* Start Header
*****************************************************************/
/*!
\file SocketAddress.h
\author Low Jin Liang Aaron, aaron.low, 390001116
\par aaron.low\@digipen.edu
\date 12/11/2019

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#ifndef _SOCKET_ADDRESS_H_
#define _SOCKET_ADDRESS_H_

#include "Sockets.h"
#include <memory>
#include <cinttypes>

class SocketAddress
{
  friend class UDPSocket;
  friend class TCPSocket;
  friend class SocketWindowData;
  friend class ConnectionManager;

  sockaddr sockAddress_; // The socket address

  sockaddr_in* GetAsSockAddrIn();

  const sockaddr_in * GetAsSockAddrIn() const;

public:
  SocketAddress(int family, uint32_t address, uint16_t port);
  SocketAddress(const sockaddr& sockAddr);
  
  inline size_t GetSize() const
  {
    return sizeof(sockAddress_);
  }
  
  void GetInfo(uint32_t& address, uint16_t& port) const;
};

using SocketAddressPtr = std::shared_ptr<SocketAddress>;

#endif
