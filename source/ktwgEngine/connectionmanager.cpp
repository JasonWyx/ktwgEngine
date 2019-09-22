#include "connectionmanager.h"
#include "socketaddress.h"
#include <vector>
#include "windowmanager.h"
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <glfw/glfw3native.h>

#define SERVER "127.0.0.1" // localhost
#define PORT 6666 // Port for listen to get new port

ConnectionManager::ConnectionManager()
  : mySocket{}, hostSocket(nullptr), host(false), shutdown(0)
{

}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::Update()
{
  // auto win = WindowManager::GetInstance().GetWindow();
  // auto hwnd = glfwGetWin32Window(win);
  // DWORD processID;
  // GetWindowThreadProcessId(hwnd, &processID);
 
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = now - then;

  SocketAddress server{ AF_INET, inet_addr(SERVER), htons(PORT) };

  // if (elapsed_seconds.count() > 5.0)
  // {
  //   then = now;
  //   
  //   std::string message = "client ";
  //   message += std::to_string(processID);
  //   message += " : Hello Server";
  //   
  //   socket.AddMessage(message);
  // 
  //   socket.DeliverMessage();
  // 
  //   // if ()
  //   // {
  //   //   std::cout << "Fail Send To" << std::endl;
  //   // }
  // }
  // 
  // ZeroMemory(buf, BUFLEN);
  // if (socket.GetSocket()->ReceiveFrom(buf, BUFLEN, server) > 0)
  // {
  //   std::cout << "Server Sent you message" << std::endl;
  // }
  mySocket.Update();
}

void ConnectionManager::InitializeInternal()
{
  SocketUtility::Init();
  ConnectToServer();
}

void ConnectionManager::ShutdownInternal()
{
  shutdown = 1;
  // serverThread.join();
  SocketUtility::CleanUp();
}

void ConnectionManager::ConnectToServer()
{
  // create udp socket
  UDPSocketPtr s = SocketUtility::CreateUDPSocket(SocketAddressFamily::IPv4);

  // setup address structure
  SocketAddress server{ AF_INET, inet_addr(SERVER), htons(PORT) };

  // create message to send to server
  std::string message = "What is the port for the new UDP socket?";
  

  // send the message
  if (s->SendTo(message.c_str(), message.size(), server) < 0)
  {
    std::cout << "Fail Send To" << std::endl;
  }

  // Clear buffer 
  ZeroMemory(buf, BUFLEN);

  // setup out address structure
  SocketAddress outserver{ AF_INET, inet_addr(SERVER), htons(PORT) };

  // receive message of new port
  if (s->ReceiveFrom(buf, BUFLEN, outserver) < 0)
  {
    std::cout << "There is no server" << std::endl;
    // Create Server and connect here

    hostSocket = SocketUtility::CreateUDPSocket(SocketAddressFamily::IPv4);
    hostSocket->SetBlockingMode(1);
    hostSocket->Bind(server);
    host = true;

    serverThread = std::thread{ ListeningServer, std::ref(hostSocket), std::ref(shutdown) };
    serverThread.detach();
    if (s->SendTo(message.c_str(), message.size(), server) < 0)
    {
      std::cout << "Send To Fail" << std::endl;
    }
    if (s->ReceiveFrom(buf, BUFLEN, outserver) < 0)
    {
      std::cout << "Fail Recieve From" << std::endl;
    }
    else
    {
      auto win = WindowManager::GetInstance().GetWindow();
      auto hwnd = glfwGetWin32Window(win);
      DWORD processID;
      GetWindowThreadProcessId(hwnd, &processID);

      std::string message = "Hello Server";

      mySocket.AddMessage(message);
      mySocket.AddMessage(message);

      u_short port = std::stoi(buf);
      std::cout << "Recieve new port to connect to as " << port << std::endl;
      then = std::chrono::system_clock::now();
      s->SetBlockingMode(1);
      mySocket.SetSocket(s);
      mySocket.SetPort(port);
    }
  }
  else
  {
    auto win = WindowManager::GetInstance().GetWindow();
    auto hwnd = glfwGetWin32Window(win);
    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);

    std::string message = "Hello Server";

    mySocket.AddMessage(message);
    mySocket.AddMessage(message);

    u_short port = std::stoi(buf);
    std::cout << "Recieve new port to connect to as " << port << std::endl;
    then = std::chrono::system_clock::now();
    s->SetBlockingMode(1);
    mySocket.SetSocket(s);
    mySocket.SetPort(port);
  }

}

void ListeningServer(UDPSocketPtr& hostSocket, int& shutdown)
{
  std::vector<SocketWindowData> serverSockets;

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
        SocketWindowData tmp;
        tmp.SetSocket(s);
        serverSockets.push_back(tmp);
      }
    }

    // Looping through all the server Sockets
    for (auto& ss : serverSockets)
    {
      ss.Update();
    }

  }
  std::cout << "Server is shutting down" << std::endl;
}

void SocketWindowData::ReadACKS(const int& acks)
{
  if (sentPkt < ackPkt + 1) return; // Duplicate ACKS
  int tmpWindowSize = windowSize;
  // int currWindowSize = windowSize;
  int bit = 0x1;
  int ackVal = acks;
  unsigned char tmpAckPkt = sentPkt - 1;
  ackPkt = tmpAckPkt;
  bool ss = true;
  std::vector<unsigned char> nackSlip;
  while (tmpWindowSize)
  {
    int currAck = ackVal & bit;
    if (!currAck)
    {
      nackSlip.push_back(tmpAckPkt);
      windowSize /= 2;
      if (windowSize <= 0) windowSize = 1;
      ss = false;
      std::cout << (int)tmpAckPkt << " is Nacked" << std::endl;
    }
    else
    {
      std::cout << (int)tmpAckPkt << " is acked" << std::endl;
    }
    --tmpAckPkt;
    --tmpWindowSize;
    bit = bit << 1;
  }

  recvPkt += windowSize;
  std::cout << "Recvied Pkts : " << (int)recvPkt << std::endl;
  SlowStart(ss);
  cumulativePktsSent = 0;
  // Send nackSlip to Stream Manager

  std::cout << "WindowSize : " << windowSize << std::endl;
}

void SocketWindowData::SlowStart(const bool& ss)
{
  if (!ss) return;
  if (windowSize < ssThres) windowSize *= 2;
  else ++windowSize;
  if (windowSize > 20) windowSize = 20;
}

void SocketWindowData::DeliverMessage()
{
  if (msgQueue.empty() || !sPort) return;
  unsigned char startPkt = sentPkt - cumulativePktsSent;
  int currWindowSize = windowSize - cumulativePktsSent;
  SocketAddress reciever{ AF_INET, inet_addr(SERVER), htons(sPort) };

  while (currWindowSize != 0)
  {
    if (msgQueue.empty()) break;
    std::string message = msgQueue.back();
    msgQueue.pop();
    message = PacketMessage(message, startPkt);
    socket->SendTo(message.c_str(), message.size(), reciever);
    --currWindowSize;
    ++cumulativePktsSent;
    ++sentPkt;
  }
  // if(cumulativePktsSent == windowSize)
  //   ackSlip.clear();
}

void SocketWindowData::ReceiveMessage()
{
  char buffer[BUFLEN];
  ZeroMemory(buffer, BUFLEN);
  SocketAddress sender{ AF_INET, inet_addr(SERVER), htons(PORT) };
  while (socket->ReceiveFrom(buffer, BUFLEN, sender) > 0)
  {
    ++dynamicRecvPkt;
    if (sPort == 0)
    {
      sPort = ntohs(sender.GetAsSockAddrIn()->sin_port);
    }

    auto message = UnPackMessage(buffer);

    // Debugging

    int portNum = (int)(std::get<0>(message));
    int startPkt = (int)(std::get<1>(message));
    int pwindowSize = std::get<2>(message);
    int Acks = std::get<3>(message);
    std::string msg = std::get<4>(message);

    std::cout << "Recieved : Packet Number : " << portNum << ", startPkt : " << startPkt << ", windowSize : " << pwindowSize << ", Acks : " << Acks <<
      " Message : " << msg << std::endl;


    if (msg == "Hello Server")
      AddMessage(std::string("Hello Client"));
    else AddMessage("Hello Server");

    // End of Debugging

    int index = ((int)std::get<0>(message)) - ((int)std::get<1>(message));
    if (index < 0) return;
    if (ackSlip.size() != std::get<2>(message))
    {
      ackSlip.clear();
      ackSlip.resize(std::get<2>(message));
      std::fill(ackSlip.begin(), ackSlip.end(), false);
    }
    if (!(!(portNum % 3) && portNum != 0)) ackSlip[index] = true;
    
    std::cout << "ACK bits " << ackSlip.size() << " ";
    for (auto b : ackSlip)
    {
      if (b) std::cout << "1 ";
      else std::cout << "0 ";
    }
    std::cout << std::endl;

    recvAckSlip = recvAckSlip | std::get<3>(message);

    // Send Message to streamManager
    if (dynamicRecvPkt == windowSize)
    {
      ReadACKS(recvAckSlip);
      dynamicRecvPkt = 0;
    }
    
    std::cout << "MSG QUEUE : " << msgQueue.size() << std::endl;
    std::cout << std::endl;

    ZeroMemory(buffer, BUFLEN);
  }

}

std::string SocketWindowData::PacketMessage(const std::string& msg, const unsigned char& startPkt)
{
  std::string message;
  message.push_back(sentPkt);
  message.push_back(startPkt);
  message.push_back((char)windowSize);
  
  int acks = GetAcks();
  char* tmp = (char*)(&acks);
  message.push_back(*tmp);
  message.push_back(*(tmp + 1));
  message.push_back(*(tmp + 2));
  message.push_back(*(tmp + 3));
  
  message += msg;
  return message;
}

void SocketWindowData::UpdateRecvAckSlip(int val, int size)
{
}

std::tuple<unsigned char, unsigned char, int, int, char*> SocketWindowData::UnPackMessage(char* msg)
{
  unsigned char pktNum = (unsigned char)(*msg);
  unsigned char startPkt = (unsigned char)(*(msg + 1));
  int sz = (int)(*(msg + 2));
  int acks = *((int*)(msg + 3));
  char* message = msg + 7;
  return std::make_tuple(pktNum, startPkt, sz, acks, message);
}

int SocketWindowData::GetAcks()
{
  int ack = 0;
  int bit = 0x1;
  for (int start = ackSlip.size() - 1; start >= 0; --start)
  {
    if (ackSlip[start]) ack += bit;
    bit = bit << 1;
  }
  return ack;
}

void SocketWindowData::AddMessage(std::string msg)
{
  msgQueue.push(msg);
}

void SocketWindowData::SetSocket(UDPSocketPtr s)
{
  socket = s;
}

void SocketWindowData::SetPort(const u_short& p)
{
  sPort = p;
}

UDPSocketPtr SocketWindowData::GetSocket()
{
  return socket;
}

void SocketWindowData::Update()
{
  DeliverMessage();
  ReceiveMessage();
}
