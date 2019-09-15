#include "connectionmanager.h"
#include "socketaddress.h"
#include <vector>
#include "windowmanager.h"
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <glfw/glfw3native.h>
#include <chrono>
#include <ctime>

#define SERVER "127.0.0.1" // localhost
#define PORT 6666 // Port for listen to get new port

ConnectionManager::ConnectionManager()
  : socket(nullptr), port(0), host(false), hostSocket(nullptr), shutdown(0)
{

}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::Update()
{
  auto win = WindowManager::GetInstance().GetWindow();
  auto hwnd = glfwGetWin32Window(win);
  DWORD processID;
  GetWindowThreadProcessId(hwnd, &processID);
 
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = now - then;

  if (elapsed_seconds.count() > 5.0)
  {
    then = now;
    SocketAddress server{ AF_INET, inet_addr(SERVER), htons(port) };
    
    std::string message = "client ";
    message += std::to_string(processID);
    message += " : Hello Server";
    
    if (socket->SendTo(message.c_str(), message.size(), server) < 0)
    {
      std::cout << "Fail Send To" << std::endl;
    }
    ZeroMemory(buf, BUFLEN);
    if (socket->ReceiveFrom(buf, BUFLEN, server) > 0)
    {
      std::cout << "Server Sent you message" << std::endl;
    }
  }

}

void ConnectionManager::InitializeInternal()
{
  SocketUtility::Init();
  ConnectToServer();
}

void ConnectionManager::ShutdownInternal()
{
  shutdown = 1;
  SocketUtility::CleanUp();
}

void ConnectionManager::ConnectToServer()
{
  // create udp socket
  socket = SocketUtility::CreateUDPSocket(SocketAddressFamily::IPv4);

  // setup address structure
  SocketAddress server{ AF_INET, inet_addr(SERVER), htons(PORT) };

  // create message to send to server
  std::string message = "What is the port for the new UDP socket?";
  

  // send the message
  if (socket->SendTo(message.c_str(), message.size(), server) < 0)
  {
    std::cout << "Fail Send To" << std::endl;
  }

  // Clear buffer 
  ZeroMemory(buf, BUFLEN);

  // setup out address structure
  SocketAddress outserver{ AF_INET, inet_addr(SERVER), htons(PORT) };

  // receive message of new port
  if (socket->ReceiveFrom(buf, BUFLEN, outserver) < 0)
  {
    std::cout << "There is no server" << std::endl;
    // Create Server and connect here
    
    hostSocket = SocketUtility::CreateUDPSocket(SocketAddressFamily::IPv4);
    hostSocket->SetBlockingMode(1);
    hostSocket->Bind(server);
    host = true;

    std::thread t (ListeningServer, hostSocket, std::ref(shutdown));
    t.detach();
    if (socket->SendTo(message.c_str(), message.size(), server) < 0)
    {
      std::cout << "Send To Fail" << std::endl;
    }
    if (socket->ReceiveFrom(buf, BUFLEN, outserver) < 0)
    {
      std::cout << "Fail Recieve From" << std::endl;
    }
    else
    {
      port = std::stoi(buf);
      std::cout << "Recieve new port to connect to as " << port << std::endl;
      then = std::chrono::system_clock::now();
      socket->SetBlockingMode(1);
    }
  }
  else
  {
    port = std::stoi(buf);
    std::cout << "Recieve new port to connect to as " << port << std::endl;
    then = std::chrono::system_clock::now();
    socket->SetBlockingMode(1);
  }

}

void ListeningServer(UDPSocketPtr hostSocket, int& shutdown)
{
  std::vector<UDPSocketPtr> serverSockets;

  std::cout << "Server : Started listening" << std::endl;
  u_short startingPort = 1234;
  // setup address structure
  SocketAddress client{ AF_INET, inet_addr(SERVER), htons(PORT) };

  char buffer[BUFLEN];

  while (!shutdown)
  {
    ZeroMemory(buffer, BUFLEN);
    if (hostSocket->ReceiveFrom(buffer, BUFLEN, client) > 0)
    {
      // std::cout << "Server: Recieve packet" << std::endl;
      // std::cout << "Server: Data : " << buffer << std::endl;

      u_short newPort = startingPort;
      std::string message = std::to_string(startingPort++);

      if (hostSocket->SendTo(message.c_str(), message.size(), client) < 0)
      {
        std::cout << "Server: Send To Fail" << std::endl;
      }
      else
      {
        UDPSocketPtr s = SocketUtility::CreateUDPSocket(SocketAddressFamily::IPv4);
        s->SetBlockingMode(1);
        SocketAddress newServer{ AF_INET, inet_addr(SERVER), htons(newPort) };
        s->Bind(newServer);
        serverSockets.push_back(s);
      }
    }

    // Looping through all the server Sockets
    for (auto ss : serverSockets)
    {
      ZeroMemory(buffer, BUFLEN);
      if (ss->ReceiveFrom(buffer, BUFLEN, client) > 0)
      {
        std::cout << "Server: Data : " << buffer << std::endl;
      }
    }

  }
}
