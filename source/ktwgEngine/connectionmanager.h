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
#include <map>

#define CLOCK_TYPE steady_clock

using TIME = std::chrono::time_point<std::chrono::CLOCK_TYPE>;

#define BUFLEN (1024 + 20)
#define BETA 0.25f
#define ALPHA 0.125f
#define MAX_WINDOW 60

class SocketWindowData
{
    typedef std::tuple<bool, TIME, float, int> PktTimer;
    float                   rtt = 1.0f;
    int                     windowSize = 30;
    unsigned char           cumulativePktsSent = 0;
    unsigned char           dynamicRecvPkt = 0;
    long long               recvAckSlip = 0;
    unsigned char           senderStartPkt = 0;
    unsigned char           sentPkt = 0;
    unsigned char           ackPkt = 0;
    unsigned char           recvPkt = 0;
    const int               ssThres = 30;
    TIME                    timer;
    float                   devRTT = 1.0f;
    u_short                 mPort = 0;
    u_short                 sPort = 0;
    UDPSocketPtr            socket;
    std::vector<bool>       ackSlip;
    std::deque<std::vector<unsigned char>> msgQueue;
    std::queue<int>         streamIDQueue;
    std::vector<PktTimer>   timeTracker;
    bool                    sentMsg = false;
    int                     timeOutPkt = 0;
    bool                    shutdown = false;
    // for server side only
    int                     player = -1;
    // to be removed if cause random DCs
    TIME                    timeOutTimer;
    std::string		        clientIP;
    bool                    notSentAckYet = true;
    TIME                    checkAckTimer = std::chrono::CLOCK_TYPE::now();

    void ReadACKS(const long long& acks);
    void SlowStart(const bool& ss);

    void ReceiveMessage();
    void UpdateTimer();
    std::vector<unsigned char> PacketMessage(const std::vector<unsigned char>& msg, const unsigned char& startPkt);
    int UpdateRecvAckSlip(long long val, int size);
    long long GetAcks();
    void DeliverMessage();
    int UpdateRecvAckSlipForAcksOnly(int val, int size);
public:
    std::tuple<unsigned char, unsigned char, int, unsigned char, long long, char*> UnPackMessage(char* msg); // to transfer to private
    void AddMessage(std::vector<unsigned char>& msg);
    void AddStreamPktID(int id);
    void SetSocket(UDPSocketPtr s);
    void SetPort(const u_short& p);
    UDPSocketPtr GetSocket();
    void Update();
    void Init();
    bool GetShutdown();
    void ShutdownMessage();
    void SetPlayer(int p);
    int  GetPlayer();
    void SetClientIP(std::string s);
};

#ifdef CLIENT
class ConnectionManager : public Singleton <ConnectionManager>
{
public:
    ConnectionManager();
    ~ConnectionManager();
    void Update();
    void AddPacket(std::vector<unsigned char> msg, int pktid);
    void RecieveMessage(std::vector<unsigned char>& msg);
    int GetPlayerID();
    std::vector<std::vector<unsigned char>>& GetRecievedMessages();
    std::vector<int>& GetLostPacketIDs();
    void StoreLostPacketsIDs(int pktid);
    void StoreAckPacketsIDs(int pktid, int p);
    std::map<int, std::vector<int>>& GetAckPacketIDs();

    void ConnectToServer();

private:
    virtual void InitializeInternal() override;
    virtual void ShutdownInternal() override;

    SocketWindowData mySocket;

    // temporary buffer for testing
    char buf[BUFLEN];
    std::vector<std::vector<unsigned char>> recievedMessages;
    std::vector<int> lostPacketIDs;
    std::map<int, std::vector<int>> ackPacketIDs;
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

    std::map<int, std::vector<std::vector<unsigned char>>> recievedMessages;
    std::vector<int> lostPacketIDs;
    std::map<int, std::vector<int>> ackPacketIDs;
    bool gameStarted = false;
public:
    ConnectionManager();
    ~ConnectionManager();
    void Update();
    void RecieveMessage(std::vector<unsigned char> msg, int p);
    void StoreLostPacketsIDs(int pktid);
    std::map<int, std::vector<std::vector<unsigned char>>>& GetRecievedMessages();
    void AddPacket(std::vector<unsigned char> msg, int pktid, int player);
    std::vector<int>& GetLostPacketIDs();
    void StoreAckPacketsIDs(int pktid, int p);
    std::map<int, std::vector<int>>& GetAckPacketIDs();
    void ShutTheFuckUp();
};

#endif