#ifndef _SOCKET_ADDRESS_FACTORY_H_
#define _SOCKET_ADDRESS_FACTORY_H_

#include "SocketAddress.h"
#include <string>

class SocketAddressFactory
{
public:
  static SocketAddressPtr CreateIPv4FromString(const std::string& inString);
  static SocketAddressPtr CreateIPv4FromString(const std::string& host, const std::string& service);
};

#endif
