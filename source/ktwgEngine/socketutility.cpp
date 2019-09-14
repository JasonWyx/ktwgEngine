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
