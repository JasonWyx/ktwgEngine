/* Start Header
*****************************************************************/
/*!
\file TCPSocket.cpp
\author Low Jin Liang Aaron, aaron.low, 390001116
\par aaron.low\@digipen.edu
\date 12/11/2019

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "TCPSocket.h"
#include "SocketAddress.h"
#include "SocketUtility.h"
#include <thread>
#include <chrono>

TCPSocket::TCPSocket(SOCKET inSocket)
  :socket_{ inSocket }
{
}

TCPSocket::~TCPSocket()
{
#if _WIN32
  // Close the socket
  closesocket(socket_);
#else
  close(socket_);
#endif
}

int TCPSocket::Connect(const SocketAddress & inAddress)
{
#ifdef _WIN32
  int result = connect(socket_, &inAddress.sockAddress_, static_cast<int>(inAddress.GetSize()));
#else
  int result = connect(socket_, &inAddress.sockAddress_, static_cast<socklen_t>(inAddress.GetSize()));
#endif

  if (result < 0)
  {
    SocketUtility::ReportError("TCPSocket::Conect");
    return -SocketUtility::GetLastError();
  }

  return NO_ERROR;
}

int TCPSocket::Bind(const SocketAddress & inAddress)
{
#if _WIN32
  auto result = bind(socket_, &inAddress.sockAddress_, static_cast<int>(inAddress.GetSize()));
#else
  auto result = bind(socket_, &inAddress.sockAddress_, static_cast<socklen_t>(inAddress.GetSize()));
#endif

  if (result != 0)
  {
    SocketUtility::ReportError("TCPSocket::Bind");
    return SocketUtility::GetLastError();
  }

  return NO_ERROR;
}

int TCPSocket::Listen(int inBackLog)
{
  int result = listen(socket_, inBackLog);

  if (result < 0)
  {
    SocketUtility::ReportError("TCPSocket::Listen");
    return -SocketUtility::GetLastError();
  }

  return NO_ERROR;
}

std::shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress & outFromAddress)
{
#ifdef _WIN32
  int len = static_cast<int>(outFromAddress.GetSize());
#else
  socklen_t len = static_cast<socklen_t>(outFromAddress.GetSize());
#endif

  SOCKET newSocket = accept(socket_, &outFromAddress.sockAddress_, &len);

  if (newSocket != INVALID_SOCKET)
  {
    return TCPSocketPtr(new TCPSocket(newSocket));
  }
  return nullptr;
}

int TCPSocket::Send(const void * inData, int inLen)
{
#if _WIN32
  int sentBytes = send(socket_, static_cast<const char*>(inData), inLen, 0);
#else
  int sentBytes = static_cast<int>(send(socket_, static_cast<const char*>(inData), inLen, 0));
#endif
  if (sentBytes < 0)
  {
    SocketUtility::ReportError("TCPSocket::Send");
    return -SocketUtility::GetLastError();
  }

  return sentBytes;
}

int TCPSocket::Receive(void * inBuffer, int inLen)
{
#ifdef _WIN32
  int receivedBytes = recv(socket_, static_cast<char*>(inBuffer), inLen, 0);
#else
  int receivedBytes = static_cast<int>(recv(socket_, static_cast<char*>(inBuffer), inLen, 0));
#endif
  if (receivedBytes < 0)
  {
    SocketUtility::ReportError("TCPSocket::Receive");
    return -SocketUtility::GetLastError();
  }

  return receivedBytes;
}

int TCPSocket::Shutdown(int inHow)
{
  return shutdown(socket_, inHow);
}

int TCPSocket::SetNonBlocking(bool inBlocking)
{
#if _WIN32
  u_long arg = static_cast<u_long>(inBlocking);
  int result = ioctlsocket(socket_, FIONBIO, &arg);
#else
  int flags = fcntl(socket_, F_GETFL, 0);
  flags = inBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
  int result = fcntl(socket_, F_SETFL, flags);
#endif

  if (result == SOCKET_ERROR)
  {
    SocketUtility::ReportError("TCPSocket::SetNonBlocking");
    return SocketUtility::GetLastError();
  }

  return NO_ERROR;
}

int TCPSocket::SetSockOpt(int inOptionName, const char* inOptionValue, socklen_t inOptionLen)
{
  int result = setsockopt(socket_, IPPROTO_TCP, inOptionName, inOptionValue, inOptionLen);

  if (result == SOCKET_ERROR)
  {
    SocketUtility::ReportError("TCPSocket::SetSockOpt");
    return SocketUtility::GetLastError();
  }

  return NO_ERROR;
}

