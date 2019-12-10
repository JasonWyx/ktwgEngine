/* Start Header
*****************************************************************/
/*!
\file SocketAddress.cpp
\author Low Jin Liang Aaron, aaron.low, 390001116
\par aaron.low\@digipen.edu
\date 12/11/2019

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "SocketAddress.h"
#include <cstring>

sockaddr_in* SocketAddress::GetAsSockAddrIn()
{
  // Reinterpret the address of sockAddress as a pointer to sockaddr_in
  return reinterpret_cast<sockaddr_in*>(&sockAddress_);
}

const sockaddr_in* SocketAddress::GetAsSockAddrIn() const
{
  // Reinterpret the address of sockAddress as a pointer to sockaddr_in
  return reinterpret_cast<const sockaddr_in*>(&sockAddress_);
}

SocketAddress::SocketAddress(int family, uint32_t address, uint16_t port)
{
  auto ptrSockAddrIn = GetAsSockAddrIn();
  
  ptrSockAddrIn->sin_family = (ADDRESS_FAMILY)family;
#if _WIN32
  ptrSockAddrIn->sin_addr.S_un.S_addr = htonl(address);
#else
  ptrSockAddrIn->sin_addr.s_addr = htonl(address);
#endif
  ptrSockAddrIn->sin_port = htons(port);
}

SocketAddress::SocketAddress(const sockaddr & sockAddr)
{
  memcpy(&sockAddress_, &sockAddr, sizeof(sockaddr));
}

void SocketAddress::GetInfo(uint32_t & address, uint16_t & port) const
{
  auto ptrSockAddrIn = GetAsSockAddrIn();

#if _WIN32
  address = ntohl(ptrSockAddrIn->sin_addr.S_un.S_addr);
#else
  address = ntohl(ptrSockAddrIn->sin_addr.s_addr);
#endif

  port = ntohs(ptrSockAddrIn->sin_port);
}
