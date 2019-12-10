/* Start Header
*****************************************************************/
/*!
\file SocketAddressFactory.cpp
\author Low Jin Liang Aaron, aaron.low, 390001116
\par aaron.low\@digipen.edu
\date 12/11/2019

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "SocketAddressFactory.h"
#include <cstring>
#include <iostream>
#include <regex>

SocketAddressPtr SocketAddressFactory::CreateIPv4FromString(const std::string & inString)
{
  // Host is the address, service is the port
  std::string host, service;

  // Find the last of : which separates the address from the port
  auto pos = inString.find_last_of(":");

  if (pos != std::string::npos)
  {
    host = inString.substr(0, pos);
    service = inString.substr(pos + 1);
  }
  else
  {
    // Set to default port 0
    host = inString;
    service = "0";
  }

  addrinfo hint;
  memset(&hint, 0, sizeof(hint));
  hint.ai_family = AF_INET;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_protocol = IPPROTO_TCP;
  
  addrinfo* result;
  auto error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);

  if (error != 0 && result != nullptr)
  {
    freeaddrinfo(result);
    return nullptr;
  }

  while (!result->ai_addr && result->ai_next)
  {
    result = result->ai_next;
  }
  if (!result->ai_addr)
  {
    freeaddrinfo(result);
    return nullptr;
  }

  auto toReturn = std::make_shared<SocketAddress>(*result->ai_addr);

  freeaddrinfo(result);

  return toReturn;
}

SocketAddressPtr SocketAddressFactory::CreateIPv4FromString(const std::string & host, const std::string & service)
{
  addrinfo hint;
  memset(&hint, 0, sizeof(hint));
  hint.ai_family = AF_INET;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_protocol = IPPROTO_TCP;

  addrinfo* result = nullptr;

  auto error = getaddrinfo(host.c_str(), service.c_str() , &hint, &result);

  if (error != 0)
  {
    if (result != nullptr)
    {
      freeaddrinfo(result);
    }
    return nullptr;
  }

  while (!result->ai_addr && result->ai_next)
  {
    result = result->ai_next;
  }
  if (!result->ai_addr)
  {
    freeaddrinfo(result);
    return nullptr;
  }

  auto toReturn = std::make_shared<SocketAddress>(*result->ai_addr);

  freeaddrinfo(result);

  return toReturn;
}
