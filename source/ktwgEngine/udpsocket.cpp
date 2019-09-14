#include "UDPSocket.h"
#include "SocketAddress.h"
#include "SocketUtility.h"

UDPSocket::UDPSocket(SOCKET inSocket)
  :socket_{ inSocket }
{
}

UDPSocket::~UDPSocket()
{
#if _WIN32
  // Close the socket
  closesocket(socket_);
#else
  close(socket_);
#endif
}

int UDPSocket::Bind(const SocketAddress& inDestinationAddress)
{
#if _WIN32
  auto result = bind(socket_, &inDestinationAddress.sockAddress_, inDestinationAddress.GetSize());
#else
  auto result = bind(socket_, &inDestinationAddress.sockAddress_, static_cast<socklen_t>(inDestinationAddress.GetSize()));
#endif

  if (result != 0)
  {
    SocketUtility::ReportError("UDPSocket::Bind");
    return SocketUtility::GetLastError();
  }

  return NO_ERROR;
}

int UDPSocket::SendTo(const void * inData, int inLen, const SocketAddress & inDestination)
{
#if _WIN32
  auto bytesSent = sendto(socket_, static_cast<const char*>(inData), inLen, 0, &inDestination.sockAddress_, inDestination.GetSize());
#else
  auto bytesSent = static_cast<int>(sendto(socket_, static_cast<const char*>(inData), inLen, 0, &inDestination.sockAddress_, static_cast<socklen_t>(inDestination.GetSize())));
#endif
  if (bytesSent >= 0)
  {
    return bytesSent;
  }
  else
  {
    SocketUtility::ReportError("UDPSocket::SendTo");
    return -SocketUtility::GetLastError();
  }
}

int UDPSocket::ReceiveFrom(void * inBuffer, int inLen, SocketAddress & outFrom)
{
#if _WIN32
  int fromLength = outFrom.GetSize();
#else
  socklen_t fromLength = static_cast<socklen_t>(outFrom.GetSize());
#endif

  int bytesReceived = static_cast<int>(recvfrom(socket_, static_cast<char*>(inBuffer), inLen, 0, &outFrom.sockAddress_, &fromLength));

  if (bytesReceived >= 0)
  {
    return bytesReceived;
  }
  else
  {
    SocketUtility::ReportError("UDPSocket::ReceiveFrom");
    return -SocketUtility::GetLastError();
  }
}
