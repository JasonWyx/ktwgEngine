#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include "singleton.h"
#include "socketutility.h"
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <vector>
#include <queue>
#include <tuple>
#include <list>
#include <stack>

#define CLOCK_TYPE steady_clock

using TIME = std::chrono::time_point<std::chrono::CLOCK_TYPE>;

#define BUFLEN 512
#define BETA 0.25f
#define ALPHA 0.125f
#define MAX_WINDOW 20

class SocketWindowData
{
  typedef std::tuple<bool, TIME, float> PktTimer;
  float                   rtt = 1.0f;
  int                     windowSize = 1;
  unsigned char           cumulativePktsSent = 0;
  unsigned char           dynamicRecvPkt = 0;
  int                     recvAckSlip = 0;
  unsigned char           senderStartPkt = 0;
  unsigned char           sentPkt = 0;
  unsigned char           ackPkt = 0;
  unsigned char           recvPkt = 0;
  const int               ssThres = 10;
  TIME                    timer;
  float                   devRTT = 1.0f;
  u_short                 mPort = 0;
  u_short                 sPort = 0;
  UDPSocketPtr            socket;
  std::vector<bool>       ackSlip;
  std::queue<std::string> msgQueue;
  std::queue<int>         streamIDQueue;
  std::stack<int>         sentStreamIDStack;
  std::vector<PktTimer>   timeTracker;
  bool                    sentMsg = false;
  int                     timeOutPkt = 0;
  bool                    shutdown = false;
  // for server side only
  int                     player = -1;
  // to be removed if cause random DCs
  TIME                    timeOutTimer;

  void ReadACKS(const int& acks);
  void SlowStart(const bool& ss);

  void ReceiveMessage();
  void UpdateTimer();
  std::string PacketMessage(const std::string & msg, const unsigned char& startPkt);
  int UpdateRecvAckSlip(int val, int size);
  int GetAcks();
  void DeliverMessage();
public:
  std::tuple<unsigned char, unsigned char, int, unsigned char, int, char*> UnPackMessage(char* msg); // to transfer to private
  void AddMessage(std::string msg);
  void AddStreamPktID(int id);
  void SetSocket(UDPSocketPtr s);
  void SetPort(const u_short& p);
  UDPSocketPtr GetSocket();
  void Update();
  void Init();
  bool GetShutdown();
  void ShutdownMessage();
  void SetPlayer(int p);
  int GetPlayer();
};

#ifdef CLIENT
class ConnectionManager : public Singleton <ConnectionManager>
{
public:
  ConnectionManager();
  ~ConnectionManager();
  void Update();
  void AddPacket(std::string msg, int pktid);
  void RecieveMessage(std::string msg);
  int GetPlayerID();
  std::vector<std::string>& GetRecievedMessages();
private:
  virtual void InitializeInternal() override;
  virtual void ShutdownInternal() override;

  void ConnectToServer();
  
  SocketWindowData mySocket;

  // temporary buffer for testing
  char buf[BUFLEN];
  std::vector<std::string> recievedMessages;
};
#else

class ConnectionManager : public Singleton<ConnectionManager>
{
  virtual void InitializeInternal() override;
  virtual void ShutdownInternal() override;

  bool playerActive[4];
  std::list<SocketWindowData> serverSockets;
  std::vector<bool> playersOnline;
  int players;

  u_short startingPort;

  char buffer[BUFLEN];

  UDPSocketPtr hostSocket;

  std::vector<std::string> recievedMessages;
public:
  ConnectionManager();
  ~ConnectionManager();
  void Update();
  void RecieveMessage(std::string msg);
  std::vector<std::string>& GetRecievedMessages();
  void AddPacket(std::string msg, int pktid, int player);
};

#endif