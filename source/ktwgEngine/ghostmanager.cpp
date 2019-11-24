#include "ghostmanager.h"
#include "streammanager.h"
#include <algorithm>

GhostManager::GhostManager()
    : m_GhostObjects()
    , m_LastPackedIndex(0)
    , m_IsDonePacking(true)
{
}

GhostManager::~GhostManager()
{
}

bool GhostManager::ProcessIncomingPacket(Packet& packet)
{
    uint16_t numObjectsInPacket = 0;
    packet.m_BitStream >> numObjectsInPacket;

    while (numObjectsInPacket > 0)
    {
        GhostID ghostNetID = 0;
        packet.m_BitStream >> ghostNetID;

        GhostObject* ghostObject = nullptr;
        auto iter = m_GhostObjectsNetIDMap.find(ghostNetID);
        if (iter != m_GhostObjectsNetIDMap.end())
        {
            // Here we process the packet straight away, but we might already have the latest data?
            // Nevermind this should already be the latest data
            ghostObject = iter->second;
            GhostStateMask stateMask(ghostObject->GetPropertyCount());
            packet.m_BitStream >> stateMask;
            ghostObject->ReadStream(packet.m_BitStream, stateMask);
        }
        else
        {
            // Object not found. Perhaps not created yet?
            _ASSERT(false);
        }

        numObjectsInPacket--;
    }
    return false;
}

bool GhostManager::ProcessOutgoingPacket(Packet& packet, TransmissionRecord& transmissionRecord)
{
    uint16_t numObjectsInPacket = 0;

    constexpr size_t headerBitCount = sizeof(numObjectsInPacket) * 8;
    const size_t currentPacketBitCount = packet.m_BitStream.GetBitLength();

    BitStream packetStream;
    BitStream objectStream;

    // Rebuild packing list for a fresh update
    if (m_IsDonePacking)
    {
        m_ObjectsToPack.clear();
        m_ObjectsToPack.reserve(m_GhostObjects.size());

        for (GhostObject* ghostObject : m_GhostObjects)
        {
            bool needUpdate = false;
            GhostStateMask stateMask = ghostObject->GetStateMaskAndCheckNeedUpdate(needUpdate);
            if (needUpdate)
            {
                m_ObjectsToPack.push_back(std::make_pair(ghostObject, stateMask));
            }
        }
        m_LastPackedIndex = 0;
    }

    for (size_t i = m_LastPackedIndex; i < m_ObjectsToPack.size(); ++i)
    {
        auto& [ghostObject, stateMask] = m_ObjectsToPack[i];
        m_CachedObjectStream.Clear();

        ghostObject->WriteStream(m_CachedObjectStream, stateMask);

        // Checking for buffer overflow...
        // HeaderSize + ObjectsStreamSize + CurrentObjectSize + CurrentPacketSize > MaxPacketBitSize
        if (headerBitCount + objectStream.GetBitLength() + m_CachedObjectStream.GetBitLength() + currentPacketBitCount > MAX_PACKET_BIT_SIZE)
        {
            return false;
        }
        else
        {
            m_LastPackedIndex++;
            numObjectsInPacket++;
            objectStream += m_CachedObjectStream;

            // Actual transmission record for each ghost object
            GhostTransmissionRecord ghostTransmissionRecord = { ghostObject->GetGhostNetID(), stateMask, nullptr };

            transmissionRecord.m_GhostTransmissionRecords.push_back(ghostTransmissionRecord);

            if (GhostTransmissionRecord* latestGhostTransmissionRecord = ghostObject->GetLatestGhostTransmissionRecord())
            {
                latestGhostTransmissionRecord->m_NextTransmissionRecord = &transmissionRecord.m_GhostTransmissionRecords.back();
            }
            ghostObject->SetLatestGhostTransmissionRecord(&transmissionRecord.m_GhostTransmissionRecords.back());
        }
    }

    packetStream << numObjectsInPacket;
    packetStream += objectStream;
    packet.m_BitStream += packetStream;

    return false;
}

void GhostManager::NotifyPacketStatus(PeerID netPeerID, PacketID packetID, PacketStatus packetStatus)
{
}

void GhostManager::DropRemainingData()
{
    m_IsDonePacking = true;
    m_LastPackedIndex = 0;
    m_CachedObjectStream.Clear();
}

void GhostManager::RegisterGhostObject(GhostObject* ghostObject)
{
    m_GhostObjects.push_back(ghostObject);
    // Jason Todo: Assign net id and register in transmission map
    m_GhostObjectsNetIDMap.try_emplace(ghostObject->GetGhostNetID(), ghostObject);
}

void GhostManager::UnregisterGhostObject(GhostObject* ghostObject)
{
    m_GhostObjects.erase(std::remove(m_GhostObjects.begin(), m_GhostObjects.end(), ghostObject));
    m_GhostObjectsNetIDMap.erase(ghostObject->GetGhostNetID());
}
