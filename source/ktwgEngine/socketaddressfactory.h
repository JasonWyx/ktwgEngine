/* Start Header
*****************************************************************/
/*!
\file SocketAddressFactory.h
\author Low Jin Liang Aaron, aaron.low, 390001116
\par aaron.low\@digipen.edu
\date 12/11/2019

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
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
