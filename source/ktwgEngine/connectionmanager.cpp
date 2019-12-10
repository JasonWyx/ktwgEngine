#include "connectionmanager.h"
#include "socketaddress.h"
#include <vector>
#include "windowmanager.h"
#include "streammanager.h"
#include "SocketAddressFactory.h"

#define SERVERIP "10.83.33.82" // localhost
#define PORT 8888 // Port for listen to get new port
#define PORTSTR "8888"

#ifdef CLIENT

ConnectionManager::ConnectionManager()
  : mySocket{}
{
}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::Update()
{
  SocketAddress server{ AF_INET, inet_addr(SERVERIP), PORT };
  mySocket.Update();
}

void ConnectionManager::RecieveMessage(std::string msg)
{
  recievedMessages.push_back(msg);
}

void ConnectionManager::StoreLostPacketsIDs(int pktid)
{
  lostPacketIDs.push_back(pktid);
}

int ConnectionManager::GetPlayerID()
{
  return mySocket.GetPlayer();
}

std::vector<std::string>& ConnectionManager::GetRecievedMessages()
{
  return recievedMessages;
}

void ConnectionManager::AddPacket(std::string msg, int pktid)
{
  mySocket.AddMessage(msg);
  mySocket.AddStreamPktID(pktid);
}

void ConnectionManager::InitializeInternal()
{
  SocketUtility::Init();
  mySocket.Init();
}

void ConnectionManager::ShutdownInternal()
{
  // debug cout
  std::cout << "Current Lost Packets Count : " << lostPacketIDs.size() << std::endl;
  mySocket.ShutdownMessage();
  SocketUtility::CleanUp();
}

void ConnectionManager::ConnectToServer()
{
  SocketAddress server{ AF_INET, inet_addr(SERVERIP), PORT };

  SocketAddressPtr sockAddr = SocketAddressFactory::CreateIPv4FromString(SERVERIP, PORTSTR);

  // create udp socket
  UDPSocketPtr s = SocketUtility::CreateUDPSocket(SocketAddressFamily::IPv4);

  // create message to send to server
  std::string message = "What is the port for the new UDP socket?";
  

  // send the message
  if (s->SendTo(message.c_str(), message.size(), *sockAddr) < 0)
  {
    std::cout << "Fail Send To" << std::endl;
  }

  // Clear buffer 
  ZeroMemory(buf, BUFLEN);

  // setup out address structure
  SocketAddress outserver{ AF_INET, inet_addr(SERVERIP), PORT };

  // receive message of new port
  int msgSize = 0;
  if ((msgSize = s->ReceiveFrom(buf, BUFLEN, outserver)) < 0)
  {
    std::cout << "There is no server" << std::endl;
    // Create Server and connect here
  }
  else
  {
    // std::string message = "Hello Server";

    // mySocket.AddMessage(message);

    std::string portMsg;
    std::string playerID;
    bool change = false;

    for (int i = 0; i < msgSize; ++i)
    {
      if (buf[i] == ',')
      {
        change = true;
        continue;
      }

      if (!change)
        portMsg.push_back(buf[i]);
      else
        playerID.push_back(buf[i]);
    }

    u_short port = std::stoi(portMsg.c_str());

    std::cout << "Recieve new port to connect to as " << port << std::endl;
    s->SetBlockingMode(1);
    mySocket.SetSocket(s);
    mySocket.SetPort(port);
    mySocket.SetPlayer(std::stoi(playerID.c_str()));
    std::cout << "I am Player " << mySocket.GetPlayer() << std::endl;
    StreamManager::GetInstance().SetPeerID(mySocket.GetPlayer());
  }
}

std::vector<int>& ConnectionManager::GetLostPacketIDs()
{
  return lostPacketIDs;
}

void ConnectionManager::StoreAckPacketsIDs(int pktid, int p)
{
  ackPacketIDs[p].push_back(pktid);
}

std::map<int, std::vector<int>>& ConnectionManager::GetAckPacketIDs()
{
  return ackPacketIDs;
}
#else
ConnectionManager::ConnectionManager()
  : players{1}, startingPort{1234}, hostSocket(nullptr)
{
  SocketUtility::Init();
  playerActive[0] = playerActive[1] = playerActive[2] = playerActive[3] = false;
  playersOnline.resize(4);
}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::Update()
{
  SocketAddress client{ AF_INET, ntohl(inet_addr(SERVERIP)), PORT };
  ZeroMemory(buffer, BUFLEN);
  if (hostSocket->ReceiveFrom(buffer, BUFLEN, client) > 0)
  {
    // std::cout << "Server: Recieve packet" << std::endl;
    // std::cout << "Server: Data : " << buffer << std::endl;

    if (players > 4)
    {
      std::cout << "Server : Max Players" << std::endl;
      return;
    }

    u_short cPort = ntohs(client.GetAsSockAddrIn()->sin_port);

    u_short newPort = startingPort;
    std::string message = std::to_string(startingPort++);
    message += ",";

    int connectedPlayerID = -1;

    if (!playerActive[0])
      connectedPlayerID = 0;
    else if (!playerActive[1])
      connectedPlayerID = 1;
    else if (!playerActive[2])
      connectedPlayerID = 2;
    else if (!playerActive[3])
      connectedPlayerID = 3;

    message += std::to_string(connectedPlayerID);

    if (hostSocket->SendTo(message.c_str(), message.size(), client) < 0)
    {
      std::cout << "Server: Send To Fail" << std::endl;
    }
    else
    {
      UDPSocketPtr s = SocketUtility::CreateUDPSocket(SocketAddressFamily::IPv4);
      s->SetBlockingMode(1);
      SocketAddress newServer{ AF_INET, INADDR_ANY, newPort };
      s->Bind(newServer);
      SocketWindowData tmp;
      tmp.Init();
      tmp.SetSocket(s);
      tmp.SetPort(cPort);
      ++players;
      tmp.SetPlayer(connectedPlayerID);
      playerActive[connectedPlayerID] = true;
      serverSockets.push_back(tmp);
      // inform upper level here
      StreamManager::GetInstance().CreatePeer(connectedPlayerID);
    }
  }

  // Looping through all the server Sockets
  for (auto& ss : serverSockets)
  {
    ss.Update();
  }

  auto ackIT = ackPacketIDs.begin();
  for (auto it = serverSockets.begin(); ackIT != ackPacketIDs.end() && it != serverSockets.end();)
  {
    int player = it->GetPlayer();
    if (it->GetShutdown())
    {
      // quit player remove player
      std::cout << "Player " << player << " has exited" << std::endl;
      it = serverSockets.erase(it);
      --players;
      // remove player here
      // inform upper level here
      playerActive[player] = false;
      ackIT = ackPacketIDs.erase(ackIT);
      std::cout << "ackPacketIDs Size : " << ackPacketIDs.size() << ", Player IDs : ";
      for (auto p : ackPacketIDs) std::cout << p.first << ", ";
      std::cout << std::endl;
      StreamManager::GetInstance().RemovePeer(player);
    }
    else
    {
      ++ackIT;
      ++it;
    }
  }
}

void ConnectionManager::InitializeInternal()
{
  // setup address structure
  // SocketAddress server{ AF_INET, inet_addr(SERVERIP), PORT };
  SocketAddress server{ AF_INET, INADDR_ANY, PORT };

  hostSocket = SocketUtility::CreateUDPSocket(SocketAddressFamily::IPv4);
  hostSocket->SetBlockingMode(1);
  hostSocket->Bind(server);
  std::cout << "Server : Started listening" << std::endl;
}

void ConnectionManager::ShutdownInternal()
{
  std::cout << "Current Lost Packets Count : " << lostPacketIDs.size() << std::endl;
  std::cout << "Server is shutting down" << std::endl;
  SocketUtility::CleanUp();
}

void ConnectionManager::RecieveMessage(std::string msg)
{
  recievedMessages.push_back(msg);
}

std::vector<std::string>& ConnectionManager::GetRecievedMessages()
{
  return recievedMessages;
}

void ConnectionManager::AddPacket(std::string msg, int pktid, int player)
{
  for (auto start = serverSockets.begin(); start != serverSockets.end(); ++start)
  {
    if (player = start->GetPlayer())
    {
      start->AddMessage(msg);
      start->AddStreamPktID(pktid);
      return;
    }
  }
}

std::vector<int>& ConnectionManager::GetLostPacketIDs()
{
  return lostPacketIDs;
}

void ConnectionManager::StoreLostPacketsIDs(int pktid)
{
  lostPacketIDs.push_back(pktid);
}

void ConnectionManager::StoreAckPacketsIDs(int pktid, int p)
{
  ackPacketIDs[p].push_back(pktid);
}

std::map<int, std::vector<int>>& ConnectionManager::GetAckPacketIDs()
{
  return ackPacketIDs;
}
#endif

void SocketWindowData::ReadACKS(const int& acks)
{
  // if (sentPkt < ackPkt + 1) return; // Duplicate ACKS
  int tmpWindowSize = windowSize;
  // int currWindowSize = windowSize;
  int bit = 0x1;
  int ackVal = acks;
  unsigned char tmpAckPkt = sentPkt - 1;
  ackPkt = tmpAckPkt;
  bool ss = true;
  // std::vector<int> nackSlip;
  std::cout << ackVal << std::endl;
  recvPkt += windowSize;
  while (tmpWindowSize)
  {
    int currAck = ackVal & bit;
    if (!currAck)
    {
      // nackSlip.push_back(tmpAckPkt);
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
  if (windowSize > MAX_WINDOW) windowSize = MAX_WINDOW;
}

void SocketWindowData::DeliverMessage()
{
  if (msgQueue.empty() || !sPort) return;
  if (!cumulativePktsSent)
  {
    timeTracker.clear();
    timeTracker.resize(windowSize);
    PktTimer tmp = std::make_tuple(true, std::chrono::CLOCK_TYPE::now(), 0.f, -1);
    std::fill(timeTracker.begin(), timeTracker.end(), tmp);
  }

  unsigned char startPkt = sentPkt - cumulativePktsSent;
  int currWindowSize = windowSize - cumulativePktsSent;
  SocketAddress reciever{ AF_INET, ntohl(inet_addr(SERVERIP)), sPort };

  while (currWindowSize != 0)
  {
    if (msgQueue.empty()) break;
    // std::cout << (int)startPkt << std::endl;
    std::string message = msgQueue.front();
    msgQueue.pop();
    int streamPktID = -1;
    if (!streamIDQueue.empty())
    {
      streamPktID = streamIDQueue.front();
      streamIDQueue.pop();
    }
    message = PacketMessage(message, startPkt);
    socket->SendTo(message.c_str(), message.size(), reciever);
    --currWindowSize;
    float timeOut = rtt + 4 * devRTT;
    timeOut = timeOut < 1.f ? 1.f : timeOut;
    PktTimer timer = std::make_tuple(false, std::chrono::CLOCK_TYPE::now(), timeOut, streamPktID);
    timeTracker[cumulativePktsSent] = timer;
    ++cumulativePktsSent;
    ++sentPkt;
    sentMsg = true;
  }

  // to be removed if cause random DCs
  if (msgQueue.empty())
    timeOutTimer = std::chrono::CLOCK_TYPE::now();
}

void SocketWindowData::ReceiveMessage()
{
  char buffer[BUFLEN];
  ZeroMemory(buffer, BUFLEN);
  SocketAddress sender{ AF_INET, ntohl(inet_addr(SERVERIP)), PORT };
  int res = 0;
  while ((res = socket->ReceiveFrom(buffer, BUFLEN, sender)) > 0)
  {
    // ++dynamicRecvPkt;
    if (sPort == 0)
    {
      sPort = ntohs(sender.GetAsSockAddrIn()->sin_port);
      std::cout << "MY PORT : " << sPort << std::endl;
    }

    std::cout << "Recieved Size is : " << res << std::endl;

    if (res == 8)
    {
      std::string signal;
      for (int i = 0; i < 8; ++i) signal.push_back(buffer[i]);
      if (signal == "shutdown")
        shutdown = true;
      else
        continue;
    }

    auto message = UnPackMessage(buffer);

    // Debugging

    int pktNum = (int)(std::get<0>(message));
    int startPkt = (int)(std::get<1>(message));
    int pwindowSize = std::get<2>(message);
    int startAckPkt = (int)(std::get<3>(message));
    int Acks = std::get<4>(message);
    std::string msg;
    for (int i = 8; i < res; ++i)
      msg.push_back(std::get<5>(message)[i]);

    std::cout << "Recieved : Packet Number : " << pktNum << ", startPkt : " << startPkt << ", windowSize : " << pwindowSize << ", startAckPkt : " << startAckPkt <<
      ", Acks : " << Acks << " Message : " << msg << std::endl;


    // if (msg == "Hello Server")
    //   AddMessage(std::string("Hello Client"));
    // else AddMessage("Hello Server");

    // End of Debugging

    // to check for duplicate delay packets a bit buggy
    // if ((int)senderStartPkt + ackSlip.size() < 255 && std::get<1>(message) < senderStartPkt)
    // {
    //   // send message to stream manager then return
    //   // --dynamicRecvPkt;
    //   continue;
    // }
    // 
    // 
    // if ((int)senderStartPkt == (int)std::get<1>(message) && std::get<2>(message) != ackSlip.size())
    // {
    //   // send message to stream manager then return
    //   // --dynamicRecvPkt;
    //   continue;
    // }
    //  
    // if ((int)std::get<1>(message) != (int)senderStartPkt && ((int)std::get<1>(message) != (((int)senderStartPkt + ackSlip.size()) % 256)))
    // {
    //   std::cout << "Start Pkt : " << (int)std::get<1>(message) << " shld be : " << (((int)senderStartPkt + ackSlip.size()) % 256) << std::endl;
    //   // send message to stream manager then return
    //   // --dynamicRecvPkt;
    //   continue;
    // }

    int index = ((int)std::get<0>(message)) - ((int)std::get<1>(message));
    // std::cout << "end size : " << (int)std::get<1>(message) + (int)std::get<2>(message) << std::endl;
    if ((int)std::get<1>(message) + (int)std::get<2>(message) > 256 && index < 0)
    {
      index += 256;
      std::cout << "index : " << index << std::endl;
    }
    if (index < 0) return;

    if (senderStartPkt != startPkt)
    {
      senderStartPkt = startPkt;
      ackSlip.clear();
      ackSlip.resize(std::get<2>(message));
      std::fill(ackSlip.begin(), ackSlip.end(), false);
    }

    // if (ackSlip[index]) 
    // {
        // send message to stream manager then return
    //   --dynamicRecvPkt;
    // }

    if (!(pktNum != 0 && !(pktNum % 9)))
    {
      ackSlip[index] = true;
      ConnectionManager::GetInstance().RecieveMessage(msg);
    }

    std::cout << "RecvPkt : " << (int)recvPkt << std::endl;

    std::cout << "ACK bits " << ackSlip.size() << " ";
    for (auto b : ackSlip)
    {
      if (b) std::cout << "1 ";
      else std::cout << "0 ";
    }
    std::cout << std::endl;

    recvAckSlip = recvAckSlip | std::get<4>(message);
    
    int tmpRecvPkts = UpdateRecvAckSlip(std::get<4>(message), windowSize);
    dynamicRecvPkt = tmpRecvPkts > dynamicRecvPkt ? tmpRecvPkts : dynamicRecvPkt;
    std::cout << "Update Recv Ack Slip : " << (int)dynamicRecvPkt << ", window size : " << windowSize << std::endl;
    // IMPT :
    // Update ack slip here with timeout
    // read the ackslip and update the dynamicRecvPkt, remove all previous dynamicRecvPkt
    // after reading the ackslip we can cfm which pkts are recieved, for each recieved pkt increment dynamicRecvPkt
    // once dynamicRecvPkt is full then we will ReadAcks and etc
    // once timeout we will also increment dynamicRecvPkt
    // may need another vector to keep track of recieved pkt, not only ack pkt
    // so when timeout, we increment the dynamicRecvPkt, and we recieved the pkt later on, we will update as we recieved
    // but wont increment the dynamicRecvPkt

    // Send Message to streamManager

    // tmp bool condition to make it work for now to be replace with the IMPT said before
    // maybe can remove
    // if (dynamicRecvPkt == windowSize && sentMsg)
    // {
    //   ReadACKS(recvAckSlip);
    //   dynamicRecvPkt = 0;
    //   recvAckSlip = 0;
    // }
    
    std::cout << "MSG QUEUE : " << msgQueue.size() << std::endl;
    std::cout << std::endl;

    ZeroMemory(buffer, BUFLEN);
  }

}

void SocketWindowData::UpdateTimer()
{
  for (int i = 0; i < timeTracker.size(); ++i)
  {
    if (i < cumulativePktsSent)
    {
      bool recv = std::get<0>(timeTracker[i]);
      if (!recv)
      {
        auto now = std::chrono::CLOCK_TYPE::now();
        const auto& then = std::get<1>(timeTracker[i]);
        float elapsedTime = std::chrono::duration<float>(std::chrono::duration_cast<std::chrono::seconds>(now - then)).count();
        float timeOut = std::get<2>(timeTracker[i]);
        int streamPktID = std::get<3>(timeTracker[i]);
        if (elapsedTime > timeOut)
        {
          PktTimer tmp = std::make_tuple(true, std::chrono::CLOCK_TYPE::now(), 0.f, streamPktID);
          ConnectionManager::GetInstance().StoreLostPacketsIDs(streamPktID);
          // store streamPktID for lost packet
          timeTracker[i] = tmp;
          ++timeOutPkt;
          std::cout << "TimeOut : " << timeOut << " elapsedTime : " << elapsedTime << std::endl;
        }
      }
    }
  }

  if ((dynamicRecvPkt + timeOutPkt == windowSize) && sentMsg)
  {
    std::cout << "DYNAMIC RECV PKT : " << (int)dynamicRecvPkt << " TIMEOUT PKT : " << timeOutPkt << " recvAckSlip : " << recvAckSlip <<std::endl;
    ReadACKS(recvAckSlip);
    dynamicRecvPkt = 0;
    recvAckSlip = 0;
    timeOutPkt = 0;
  }
  // ungracefull disconnection
  if (timeOutPkt == windowSize)
  {
    shutdown = true;
  }

}

std::string SocketWindowData::PacketMessage(const std::string& msg, const unsigned char& startPkt)
{
  std::string message;
  message.push_back(sentPkt);
  message.push_back(startPkt);
  message.push_back((char)windowSize);
  message.push_back(senderStartPkt);
  
  int acks = GetAcks();
  char* tmp = (char*)(&acks);
  message.push_back(*tmp);
  message.push_back(*(tmp + 1));
  message.push_back(*(tmp + 2));
  message.push_back(*(tmp + 3));
  
  message += msg;
  return message;
}

int SocketWindowData::UpdateRecvAckSlip(int val, int size)
{
  int recvPkts = 0;
  int bit = 0x1;
  // PktTimer tmp = std::make_tuple(true, std::chrono::CLOCK_TYPE::now(), 0.f);
  for (int i = size - 1; i >= 0; --i)
  {
    int result = val & bit;
    if (result)
    {
      ++recvPkts;
      bool recv = std::get<0>(timeTracker[i]);
      if (i < cumulativePktsSent)
      {
        auto now = std::chrono::CLOCK_TYPE::now();
        auto then = std::get<1>(timeTracker[i]);
        int streamPktID = std::get<3>(timeTracker[i]);
        float elapsedTime = std::chrono::duration<float>(std::chrono::duration_cast<std::chrono::seconds>(now - then)).count();
        //std::cout << "RTT' :" << elapsedTime << std::endl;
        devRTT = (1.f - BETA) * devRTT + BETA * std::fabs((float)elapsedTime - rtt);
        rtt = (1.f - ALPHA) * (float)elapsedTime + ALPHA * rtt;
        PktTimer tmp = std::make_tuple(true, std::chrono::CLOCK_TYPE::now(), 0.f, streamPktID);
        //std::cout << "DevRtt : " << devRTT << std::endl;
        //std::cout << "RTT : " << rtt << std::endl;
        timeTracker[i] = tmp;
        ConnectionManager::GetInstance().StoreAckPacketsIDs(streamPktID, player);
      }
    }
    bit = bit << 1;
  }
  return recvPkts;
}

std::tuple<unsigned char, unsigned char, int, unsigned char, int, char*> SocketWindowData::UnPackMessage(char* msg)
{
  unsigned char pktNum = (unsigned char)(*msg);
  unsigned char startPkt = (unsigned char)(*(msg + 1));
  int sz = (int)(*(msg + 2));
  unsigned char startAckPkt = (unsigned char)(*(msg + 3));
  int acks = *((int*)(msg + 4));
  char* message = msg + 8;
  return std::make_tuple(pktNum, startPkt, sz, startAckPkt, acks, message);
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

void SocketWindowData::AddStreamPktID(int id)
{
  streamIDQueue.push(id);
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
  UpdateTimer();

  // to be removed if cause random DCs
  if (msgQueue.empty() && sentMsg)
  {
    auto current = std::chrono::CLOCK_TYPE::now();
    float elapsedTime = std::chrono::duration<float>(std::chrono::duration_cast<std::chrono::seconds>(current - timeOutTimer)).count();
    if (elapsedTime > 10.f)
    {
      std::cout << "Player " << player << " has Disconnected" << std::endl;
      shutdown = true;
    }
  }
}

void SocketWindowData::Init()
{
  ackSlip.resize(1);
  ackSlip[0] = false;
}

bool SocketWindowData::GetShutdown()
{
  return shutdown;
}

void SocketWindowData::ShutdownMessage()
{
  while(!msgQueue.empty())
    msgQueue.pop();
  std::string s = "shutdown";
  SocketAddress reciever{ AF_INET, ntohl(inet_addr(SERVERIP)), sPort };
  socket->SendTo(s.c_str(), s.size(), reciever);
}

void SocketWindowData::SetPlayer(int p)
{
  player = p;
}

int SocketWindowData::GetPlayer()
{
  return player;
}
