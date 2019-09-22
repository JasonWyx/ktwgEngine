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

using TIME = std::chrono::time_point<std::chrono::system_clock>;

#define BUFLEN 512

void ListeningServer(UDPSocketPtr& hostSocket, int& shutdown);

class SocketWindowData
{
  float                   rtt;
  int                     windowSize = 1;
  int                     cumulativePktsSent = 0;
  int                     dynamicRecvPkt = 0;
  int                     recvAckSlip = 0;
  unsigned char           senderStartPkt = 0;
  unsigned char           sentPkt = 0;
  unsigned char           ackPkt = 0;
  unsigned char           recvPkt = 0;
  const int               ssThres = 10;
  TIME                    timer;
  TIME                    devRTT;
  u_short                 mPort = 0;
  u_short                 sPort = 0;
  UDPSocketPtr            socket;
  std::vector<bool>       ackSlip;
  std::queue<std::string> msgQueue;

  void ReadACKS(const int& acks);
  void SlowStart(const bool& ss);

  void ReceiveMessage();
  std::string PacketMessage(const std::string & msg, const unsigned char& startPkt);
  void UpdateRecvAckSlip(int val, int size);
  int GetAcks();
public:
  void DeliverMessage(); // to transfer to private
  std::tuple<unsigned char, unsigned char, int, int, char*> UnPackMessage(char* msg); // to transfer to private
  void AddMessage(std::string msg);
  void SetSocket(UDPSocketPtr s);
  void SetPort(const u_short& p);
  UDPSocketPtr GetSocket();
  void Update();
};

class ConnectionManager : public Singleton <ConnectionManager>
{
public:
  ConnectionManager();
  ~ConnectionManager();
  void Update();
private:
  virtual void InitializeInternal() override;
  virtual void ShutdownInternal() override;

  void ConnectToServer();
  
  SocketWindowData mySocket;
  UDPSocketPtr hostSocket;
  std::thread serverThread;
  int shutdown;
  bool host;

  // temporary buffer for testing
  char buf[BUFLEN];
  std::chrono::system_clock::time_point then;

};