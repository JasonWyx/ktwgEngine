#include "SocketAddress.h"
#include <cstring>

sockaddr_in* SocketAddress::GetAsSockAddrIn()
{
  // Reinterpret the address of sockAddress as a pointer to sockaddr_in
  return reinterpret_cast<sockaddr_in*>(&sockAddress_);
}

SocketAddress::SocketAddress(int family, uint32_t address, uint16_t port)
{
  auto ptrSockAddrIn = GetAsSockAddrIn();
  
  ptrSockAddrIn->sin_family = (ADDRESS_FAMILY)family;
#if _WIN32
  ptrSockAddrIn->sin_addr.S_un.S_addr = address;
#else
  ptrSockAddrIn->sin_addr.s_addr = htonl(address);
#endif
  ptrSockAddrIn->sin_port = port;
}

SocketAddress::SocketAddress(const sockaddr & sockAddr)
{
  memcpy(&sockAddress_, &sockAddr, sizeof(sockaddr));
}
