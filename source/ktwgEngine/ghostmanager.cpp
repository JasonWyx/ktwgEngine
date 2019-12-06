#include "ghostmanager.h"
#include "streammanager.h"
#include <algorithm>
#include <cassert>

GhostManager::GhostManager()
    : m_GhostObjects()
    , m_LastPackedIndex(0)
    , m_IsDonePacking(true)
{
}

GhostManager::~GhostManager()
{
}

bool GhostManager::ReadStream(BitStream& stream)
{
    uint16_t countObjectsInPacket = 0;
    stream >> countObjectsInPacket;

    while (countObjectsInPacket > 0)
    {
        GhostID ghostID = 0xFFFFFFFF;
        stream >> ghostID;

        // Check for status change
        bool isStatusChanged = false;
        bool isCreateNew = false;

        stream >> isStatusChanged;
        if (isStatusChanged)
        {
            stream >> isCreateNew;
            if (isCreateNew)
            {
                // Create new object with class id, with server as the authority
                uint32_t ghostClassID = 0;
                stream.Read(ghostClassID, 0u);
                // jason todo: create object here with m_IsLocalOwner set to false in ghost object so
                // that local client does not send updates to server for broadcast
				
				// read data here and send to scene to create
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
            stream >> stateMask;
            ghostObject->ReadStream(stream, stateMask);
        }
        else
        {
            // Object not found. Perhaps not created yet?
            assert(false);
        }

        countObjectsInPacket--;
    }
    return true;
}

bool GhostManager::WriteStream(BitStream& stream, TransmissionRecord& tr, const size_t packetSizeInBits)
{
    uint16_t countObjectsInPacket = 0;
    const size_t headerSizeInBits = sizeof(countObjectsInPacket) * 8;

    BitStream ghostObjectStream; // This one only has the actual objects in the stream

    // Rebuild packing list for a fresh update
    if (m_IsDonePacking)
    {
        m_ObjectsToPack.clear();
        m_ObjectsToPack.reserve(m_GhostObjects.size());
        m_LastPackedIndex = 0;

        for (GhostID ghostID : m_GhostsToCreate)
        {
            if (GhostObject* ghostObject = m_GhostObjectsIDMap[ghostID])
            {
                m_ObjectsToPack.emplace_back(ghostObject, ghostObject->GetFullStateMask());
            }
        }

        for (GhostObject* ghostObject : m_GhostObjects)
        {
            // Object has status change, disregard as it is already packed above
            if (m_GhostsToCreate.find(ghostObject->GetGhostID()) != m_GhostsToCreate.end() ||
                m_GhostsToDelete.find(ghostObject->GetGhostID()) != m_GhostsToDelete.end())
            {
                continue;
            }
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
            stream << countObjectsInPacket;
            stream += ghostObjectStream;
            return false;
        }
    }

    // Successfully packed everthing into the current packet
    stream << countObjectsInPacket;
    stream += ghostObjectStream;
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

        // Append to corresponding status change list and let the packing method handle the rest
        if (gtr.m_Status == GhostStatusType::Create)
        {
            m_GhostsToCreate.insert(gtr.m_GhostID);
            continue;
        }
        else if (gtr.m_Status == GhostStatusType::Delete)
        {
            m_GhostsToDelete.insert(gtr.m_GhostID);
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

#ifdef CLIENT
        ghostObject->SetRetransmissionMask(gtr.m_StateMask);
#else
        ghostObject->SetRetransmissionMask(tr.m_PeerID, gtr.m_StateMask);
#endif
    }
}

void GhostManager::SyncObjectPropertyValues()
{
    for (GhostObject* ghostObject : m_GhostObjects)
    {
        ghostObject->SyncPropertyValues();
    }
}

void GhostManager::DropPendingData()
{
#ifdef CLIENT
    m_PackingInfo.m_CachedObjectStream.Clear();
    m_PackingInfo.m_IsDonePacking = true;
    m_PackingInfo.m_LastPackedIndex = 0;
    m_PackingInfo.m_ObjectsToPack.clear();
#else
    for (PackingInfo& packingInfo : m_PackingInfo)
    {
        packingInfo.m_CachedObjectStream.Clear();
        packingInfo.m_IsDonePacking = true;
        packingInfo.m_LastPackedIndex = 0;
        packingInfo.m_ObjectsToPack.clear();
    }
#endif
}

void GhostManager::ClearStatusChanges()
{
    m_GhostsToCreate.clear();
    m_GhostsToDelete.clear();
}

void GhostManager::RegisterGhostObject(GhostObject* ghostObject)
{
    m_GhostObjects.push_back(ghostObject);
    m_GhostObjectsIDMap.try_emplace(ghostObject->GetGhostID(), ghostObject);
}

void GhostManager::UnregisterGhostObject(GhostObject* ghostObject)
{
    m_GhostObjects.erase(std::remove(m_GhostObjects.begin(), m_GhostObjects.end(), ghostObject));
    m_GhostObjectsIDMap.erase(ghostObject->GetGhostID());
}
