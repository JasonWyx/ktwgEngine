/* Start Header
*****************************************************************/
/*!
\file ScoketUtility.cpp
\author Low Jin Liang Aaron, aaron.low, 390001116
\par aaron.low\@digipen.edu
\date 12/11/2019

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "SocketUtility.h"
#include <iostream>


bool SocketUtility::Init()
{
#if _WIN32
  WSAData wsadata;
  
  auto result = WSAStartup(MAKEWORD(2, 2), &wsadata);

  if (result != NO_ERROR)
  {
    ReportError("SocketUtility::Init");
    return false;
  }

#endif
  return true;
}

void SocketUtility::CleanUp()
{
#if _WIN32
  WSACleanup();
#endif
}

void SocketUtility::ReportError(const char * inOpDesc)
{
#if PRINTDEBUG

#if _WIN32
  LPVOID lpMsgBuffer;

  auto errorNumber = GetLastError();

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorNumber,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&lpMsgBuffer,
                0,
                NULL);

  std::cout << "Error " << inOpDesc << ": " << errorNumber << " " << lpMsgBuffer << "\n";
#else
  std::cout << "Error " << inOpDesc << ": " << GetLastError();
#endif
#else
  UNREFERENCED_PARAMETER(inOpDesc);

#endif
}

int SocketUtility::GetLastError()
{
#if _WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}

UDPSocketPtr SocketUtility::CreateUDPSocket(SocketAddressFamily inFamily)
{
  SOCKET sock = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);

  if (sock != SOCKET_ERROR)
  {
    return UDPSocketPtr{ new UDPSocket{ sock } };
  }

  ReportError("SocketUtility::CreateUDPSocket");
  return nullptr;
}

TCPSocketPtr SocketUtility::CreateTCPSocket(SocketAddressFamily inFamily)
{
  SOCKET sock = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);

  if (sock != SOCKET_ERROR)
  {
    return TCPSocketPtr{ new TCPSocket{sock} };
  }

  ReportError("SocketUtility::CreateTCPSocket");
  return nullptr;
}

fd_set* SocketUtility::FillSetFromVector(fd_set & outSet, const std::vector<TCPSocketPtr>* inSockets, int* nfds)
{
  if (inSockets)
  {
    FD_ZERO(&outSet);
    for (auto& socket : *inSockets)
    {
      FD_SET(socket->socket_, &outSet);
#if _WIN32
      UNREFERENCED_PARAMETER(nfds);
#else
      if (nfds && *nfds < socket->socket_)
        *nfds = socket->socket_;
#endif
    }
    return &outSet;
  }
  else
  {
    return nullptr;
  }
}

void SocketUtility::FillVectorFromSet(std::vector<TCPSocketPtr>* outSockets, const std::vector<TCPSocketPtr>* inSockets, const fd_set & inSet)
{
  if (outSockets && inSockets)
  {
    outSockets->clear();

    for (auto& socket : *inSockets)
    {
      if (FD_ISSET(socket->socket_, &inSet))
      {
        outSockets->emplace_back(socket);
      }
    }
  }
}

int SocketUtility::Select(const std::vector<TCPSocketPtr>* inReadSet, std::vector<TCPSocketPtr>* outReadSet, const std::vector<TCPSocketPtr>* inWriteSet, std::vector<TCPSocketPtr>* outWriteSet, const std::vector<TCPSocketPtr>* inExcepSet, std::vector<TCPSocketPtr>* outExcepSet, timeval* timeout)
{
  fd_set read, write, excep;

#if _WIN32

  fd_set* pRead = FillSetFromVector(read, inReadSet);
  fd_set* pWrite = FillSetFromVector(write, inWriteSet);
  fd_set* pExcep = FillSetFromVector(excep, inExcepSet);

  int res = select(0, pRead, pWrite, pExcep, timeout);
#else
  int nfds = 0;

  fd_set* pRead = FillSetFromVector(read, inReadSet, &nfds);
  fd_set* pWrite = FillSetFromVector(write, inWriteSet, &nfds);
  fd_set* pExcep = FillSetFromVector(excep, inExcepSet, &nfds);

  int res = select(nfds + 1, pRead, pWrite, pExcep, timeout);
#endif

  if (res > 0)
  {
    FillVectorFromSet(outReadSet, inReadSet, read);
    FillVectorFromSet(outWriteSet, inWriteSet, write);
    FillVectorFromSet(outExcepSet, inExcepSet, excep);
  }

  return res;
}

fd_set* SocketUtility::FillSetFromVector(fd_set & outSet, const std::vector<UDPSocketPtr>* inSockets, int* nfds)
{
  if (inSockets)
  {
    FD_ZERO(&outSet);
    for (auto& socket : *inSockets)
    {
      FD_SET(socket->socket_, &outSet);
#if _WIN32
      UNREFERENCED_PARAMETER(nfds);
#else
      if(nfds && *nfds < socket->socket_)
        *nfds = socket->socket_;
#endif
    }
    return &outSet;
  }
  else
  {
    return nullptr;
  }
}

void SocketUtility::FillVectorFromSet(std::vector<UDPSocketPtr>* outSockets, const std::vector<UDPSocketPtr>* inSockets, const fd_set & inSet)
{
  if (outSockets && inSockets)
  {
    outSockets->clear();

    for (auto& socket : *inSockets)
    {
      if (FD_ISSET(socket->socket_, &inSet))
      {
        outSockets->emplace_back(socket);
      }
    }
  }
}

int SocketUtility::Select(const std::vector<UDPSocketPtr>* inReadSet, std::vector<UDPSocketPtr>* outReadSet, const std::vector<UDPSocketPtr>* inWriteSet, std::vector<UDPSocketPtr>* outWriteSet, const std::vector<UDPSocketPtr>* inExcepSet, std::vector<UDPSocketPtr>* outExcepSet, timeval* timeout)
{
  fd_set read, write, excep;

#if _WIN32

  fd_set* pRead = FillSetFromVector(read, inReadSet);
  fd_set* pWrite = FillSetFromVector(write, inWriteSet);
  fd_set* pExcep = FillSetFromVector(excep, inExcepSet);

  int res = select(0, pRead, pWrite, pExcep, timeout);
#else
  int nfds = 0;

  fd_set* pRead = FillSetFromVector(read, inReadSet, &nfds);
  fd_set* pWrite = FillSetFromVector(write, inWriteSet, &nfds);
  fd_set* pExcep = FillSetFromVector(excep, inExcepSet, &nfds);

  int res = select(nfds + 1, pRead, pWrite, pExcep, timeout);
#endif

  if (res > 0)
  {
    FillVectorFromSet(outReadSet, inReadSet, read);
    FillVectorFromSet(outWriteSet, inWriteSet, write);
    FillVectorFromSet(outExcepSet, inExcepSet, excep);
  }

  return res;
}
