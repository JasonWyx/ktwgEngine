#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include "singleton.h"
#include "socketutility.h"
#include <string>
#include <iostream>
#include <thread>

#define BUFLEN 512

void ListeningServer(UDPSocketPtr hostSocket, int& shutdown);

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
  
  UDPSocketPtr socket;
  UDPSocketPtr hostSocket;
  u_short port;
  int shutdown;
  bool host;

  // temporary buffer for testing
  char buf[BUFLEN];
  std::chrono::system_clock::time_point then;

};