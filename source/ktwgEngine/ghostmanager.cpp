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

bool GhostManager::ReadPacket(Packet& packet)
{
    uint16_t countObjectsInPacket = 0;
    packet.m_BitStream >> countObjectsInPacket;

    while (countObjectsInPacket > 0)
    {
        GhostID ghostID = 0xFFFFFFFF;
        packet.m_BitStream >> ghostID;

        // Check for status change
        bool isStatusChanged = false;
        bool isCreateNew = false;

        packet.m_BitStream >> isStatusChanged;
        if (isStatusChanged)
        {
            packet.m_BitStream >> isCreateNew;
            if (isCreateNew)
            {
                // Create new object with class id, with server as the authority
                uint32_t ghostClassID = 0;
                packet.m_BitStream.Read(ghostClassID, 0u);
                // jason todo: create object here with m_IsLocalOwner set to false in ghost object so
                // that local client does not send updates to server for broadcast
            }
            else
            {
                // jason todo: Flag object for removal and move on to the next one, no state data present
                continue;
            }
        }

        // No status change, just do as per normal
        GhostObject* ghostObject = nullptr;
        auto iter = m_GhostObjectsIDMap.find(ghostID);
        if (iter != m_GhostObjectsIDMap.end())
        {
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

        countObjectsInPacket--;
    }
    return true;
}

bool GhostManager::WritePacket(Packet& packet, TransmissionRecord& tr)
{
    uint16_t countObjectsInPacket = 0;
    const size_t headerSizeInBits = sizeof(countObjectsInPacket) * 8;
    const size_t packetSizeInBits = packet.m_BitStream.GetBitLength();

    BitStream ghostObjectStream; // This one only has the actual objects in the stream

    // Rebuild packing list for a fresh update
    if (m_IsDonePacking)
    {
        m_ObjectsToPack.clear();
        m_ObjectsToPack.reserve(m_GhostObjects.size());
        m_LastPackedIndex = 0;

        for (GhostObject* ghostObject : m_GhostObjects)
        {
            bool needUpdate = false;
            GhostStateMask stateMask = ghostObject->GetStateMaskAndCheckNeedUpdate(needUpdate);
            if (needUpdate)
            {
                m_ObjectsToPack.push_back(std::make_pair(ghostObject, stateMask));
            }
        }
    }
    else if(headerSizeInBits + packetSizeInBits + m_CachedObjectStream.GetBitLength() <= MAX_PACKET_BIT_SIZE)
    {
        // jason todo: check if the current cached object can go into the packet
        // if not, return false again
        ghostObjectStream += m_CachedObjectStream;
    }
    else // No space even for 1 object (with header included)
    {
        return false;
    }

    /*
        In this loop, we build the ghost object stream first, then we check if the size can fit.
        1. If it fits, we append to the objects stream 
        2. If it does not fit, we append what we have to the packet with the object count and return false
    */
    for (size_t i = m_LastPackedIndex; i < m_ObjectsToPack.size(); ++i)
    {
        // Packet structure [GhostID][StatusChange][StateMask][StateData]
        m_CachedObjectStream.Clear();

        auto& [ghostObject, stateMask] = m_ObjectsToPack[i];

        m_CachedObjectStream << ghostObject->GetGhostID();

        // Check if there is a status change; need to send create/destroy status change
        GhostStatusType status = GhostStatusType::None;
        if (m_GhostsToCreate.find(ghostObject->GetGhostID()) != m_GhostsToCreate.end())
        {
            // Object is flagged for creation
            m_CachedObjectStream << true << true;
            m_CachedObjectStream.Write(0u, 4); // jason todo: replace 0u with ghost class id
            status = GhostStatusType::Create;
        }
        else if (m_GhostsToDelete.find(ghostObject->GetGhostID()) != m_GhostsToDelete.end())
        {
            // Object is flagged for removal, do not need to pack state data
            m_CachedObjectStream << true << false;
            status = GhostStatusType::Delete;            
        }
        else
        {
            // No status change, send states as per normal
            m_CachedObjectStream << false;
            ghostObject->WriteStream(m_CachedObjectStream, stateMask);
        }

        // Checking for buffer overflow...
        // HeaderSize + ObjectsStreamSize + CurrentObjectSize + CurrentPacketSize > MaxPacketBitSize
        if (headerSizeInBits + ghostObjectStream.GetBitLength() + m_CachedObjectStream.GetBitLength() + packetSizeInBits <= MAX_PACKET_BIT_SIZE)
        {
            m_LastPackedIndex++;
            countObjectsInPacket++;
            ghostObjectStream += m_CachedObjectStream;

            // Create new transmission record for object
            GhostTransmissionRecord& gtr = tr.m_GhostTransmissionRecords.emplace_back();
            gtr.m_GhostID = ghostObject->GetGhostID();
            gtr.m_StateMask = stateMask;
            gtr.m_Status = status;

            if (GhostTransmissionRecord* latestGhostTransmissionRecord = ghostObject->GetLatestGhostTransmissionRecord())
            {
                gtr.m_PreviousTransmissionRecord = latestGhostTransmissionRecord;
                latestGhostTransmissionRecord->m_NextTransmissionRecord = &tr.m_GhostTransmissionRecords.back();
            }
            ghostObject->SetLatestGhostTransmissionRecord(&gtr);
        } 
        else
        { 
            // Packet what ever we have and notify that we still have more ghost objects to pack
            packet.m_BitStream << countObjectsInPacket;
            packet.m_BitStream += ghostObjectStream;
            return false;
        }
    }

    // Successfully packed everthing into the current packet
    packet.m_BitStream << countObjectsInPacket;
    packet.m_BitStream += ghostObjectStream;
    return true;
}

void GhostManager::NotifyTransmissionSuccess(TransmissionRecord& tr)
{
    // Relink transmission records
    for (GhostTransmissionRecord& gtr : tr.m_GhostTransmissionRecords)
    {
        if (GhostTransmissionRecord* previous = gtr.m_PreviousTransmissionRecord)
        {
            previous->m_NextTransmissionRecord = gtr.m_NextTransmissionRecord;
        }

        if (GhostTransmissionRecord* next = gtr.m_NextTransmissionRecord)
        {
            next->m_PreviousTransmissionRecord = gtr.m_PreviousTransmissionRecord;
        }
    }
}

void GhostManager::NotifyTransmissionFailure(TransmissionRecord& tr)
{
    for (GhostTransmissionRecord& gtr : tr.m_GhostTransmissionRecords)
    {
        // Check for valid object, if object is already deleted then we don't bother
        auto iter = m_GhostObjectsIDMap.find(gtr.m_GhostID);
        if (iter == m_GhostObjectsIDMap.end())
        {
            continue;
        }

        GhostObject* ghostObject = iter->second;
        if (ghostObject == nullptr)
        {
            continue;
        }

        // Check against next transmission records for already transmitted states
        if (GhostTransmissionRecord* nextRecord = gtr.m_NextTransmissionRecord)
        {
            while (nextRecord != nullptr)
            {
                for (size_t i = 0; i < nextRecord->m_StateMask.size(); ++i)
                {
                    // Set the state to false in the current transmission record so we do not process this again
                    if (nextRecord->m_StateMask[i] == true)
                    {
                        gtr.m_StateMask[i] = false;
                    }
                }

                nextRecord = nextRecord->m_NextTransmissionRecord;
            }
        }

        // Relink transmission record as this record will be removed
        if (GhostTransmissionRecord* previous = gtr.m_PreviousTransmissionRecord)
        {
            previous->m_NextTransmissionRecord = gtr.m_NextTransmissionRecord;
        }

        if (GhostTransmissionRecord* next = gtr.m_NextTransmissionRecord)
        {
            next->m_PreviousTransmissionRecord = gtr.m_PreviousTransmissionRecord;
        }

        ghostObject->SetRetransmissionMask(gtr.m_StateMask);
    }
}

void GhostManager::DropPendingData()
{
    m_IsDonePacking = true;
    m_LastPackedIndex = 0;
    m_CachedObjectStream.Clear();
}

void GhostManager::RegisterGhostObject(GhostObject* ghostObject)
{
    m_GhostObjects.push_back(ghostObject);
    // Jason Todo: Assign net id and register in transmission map
    m_GhostObjectsIDMap.try_emplace(ghostObject->GetGhostID(), ghostObject);
}

void GhostManager::UnregisterGhostObject(GhostObject* ghostObject)
{
    m_GhostObjects.erase(std::remove(m_GhostObjects.begin(), m_GhostObjects.end(), ghostObject));
    m_GhostObjectsIDMap.erase(ghostObject->GetGhostID());
}
