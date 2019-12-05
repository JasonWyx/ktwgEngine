#include "streammanager.h"

StreamManager::StreamManager()
    : m_ClientStreamManager(nullptr)
    , m_ServerStreamManager(nullptr)
    , m_PeerID(0)
    , m_LastPacketID(0)
    , m_FramePacketCount(0)
    , m_MaxFramePacketCount(1000)
{
}

StreamManager::~StreamManager()
{
}

void StreamManager::Update()
{
    if (IsServer())
    {

    }
    else
    {
		m_ClientStreamManager->Update();
    }
}

bool StreamManager::SendPacket(Packet& packet)
{
    return false;
}

bool StreamManager::ReceivePacket(Packet& packet)
{
    return false;
}

void StreamManager::NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus)
{
}

Packet StreamManager::CreatePacket()
{
    return Packet{ m_LastPacketID++ };
}

void StreamManager::InitializeClient()
{ 
    m_ClientStreamManager = new StreamManagerClient();
}

void StreamManager::ShutdownClient()
{
    delete m_ClientStreamManager;
}

void StreamManager::UpdateClient()
{
    m_ClientStreamManager->Update();
}

void StreamManager::InitializeServer()
{
    m_ServerStreamManager = new StreamManagerServer(m_ClientStreamManager);
}

void StreamManager::ShutdownServer()
{
    delete m_ServerStreamManager;
}

void StreamManager::UpdateServer()
{
    m_ServerStreamManager->Update();
}

void StreamManager::InitializeInternal()
{
}

void StreamManager::ShutdownInternal()
{
}
