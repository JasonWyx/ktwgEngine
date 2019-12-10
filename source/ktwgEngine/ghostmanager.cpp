#include "ghostmanager.h"
#include "streammanager.h"
#include "scene.h"
#include <algorithm>
#include <cassert>

GhostManager::GhostManager()
    : m_GhostObjects()
{
}

GhostManager::~GhostManager()
{
}

void GhostManager::GenerateGhostIDs()
{
    PeerID peerID = StreamManager::GetInstance().GetPeerID();

    m_AvailableGhostIDs = std::queue<GhostID>();

    for (GhostID i = 0; i < 1 << 13; ++i)
    {
        m_AvailableGhostIDs.push(static_cast<GhostID>(peerID) << 12 & i);
    }
}

GhostID GhostManager::GetAvailableGhostID()
{
    GhostID ghostID = m_AvailableGhostIDs.front();
    m_AvailableGhostIDs.pop();

    return ghostID;
}

void GhostManager::ReadStream(BitStream& stream)
{
    uint16_t countObjectsInPacket = 0;
    stream >> countObjectsInPacket;

    while (countObjectsInPacket > 0)
    {
        GhostID ghostID = 0xFFFF;
        stream >> ghostID;
        PeerID peerID = ghostID >> 13;

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

                // jason todo: create object here with m_IsLocalOwner set to false in ghost object so
                // that local client does not send updates to server for broadcast
				
				        // read data here and send to scene to create
#if CLIENT
              Scene::GetInstance().CreateGhostEntity(stream);
#endif
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
}

bool GhostManager::WritePacket(Packet& packet, TransmissionRecord& tr)
{
    uint16_t countObjectsInPacket = 0;
    const size_t headerSizeInBits = sizeof(countObjectsInPacket) * 8;
    const size_t packetSizeInBits = packet.GetStreamSize();

    BitStream ghostObjectStream; // This one only has the actual objects in the stream

    // Rebuild packing list for a fresh update

#ifdef CLIENT
    PackingInfo& packingInfo = m_PackingInfo;
#else
    PackingInfo& packingInfo = m_PackingInfo[tr.m_TargetPeerID];
#endif

    if (packingInfo.m_IsDonePacking)
    {
        packingInfo.m_ObjectsToPack.clear();
        packingInfo.m_ObjectsToPack.reserve(m_GhostObjects.size());
        packingInfo.m_LastPackedIndex = 0;

        for (GhostID ghostID : packingInfo.m_GhostsToCreate)
        {
            if (GhostObject* ghostObject = m_GhostObjectsIDMap[ghostID])
            {
                packingInfo.m_ObjectsToPack.emplace_back(ghostID, ghostObject->GetFullStateMask());
            }
        }

        for (GhostID ghostID : packingInfo.m_GhostsToDelete)
        {
            packingInfo.m_ObjectsToPack.emplace_back(ghostID, GhostStateMask());
        }

        for (GhostObject* ghostObject : m_GhostObjects)
        {
            // Object has status change, disregard as it is already packed above
            if (packingInfo.m_GhostsToCreate.find(ghostObject->GetGhostID()) != packingInfo.m_GhostsToCreate.end() ||
                packingInfo.m_GhostsToDelete.find(ghostObject->GetGhostID()) != packingInfo.m_GhostsToDelete.end())
            {
                continue;
            }
            bool needUpdate = false;

#ifdef CLIENT
            GhostStateMask stateMask = ghostObject->GetStateMaskAndCheckNeedUpdate(needUpdate);
#else
            GhostStateMask stateMask = ghostObject->GetStateMaskAndCheckNeedUpdate(tr.m_TargetPeerID, needUpdate);
#endif
            if (needUpdate)
            {
                packingInfo.m_ObjectsToPack.emplace_back(ghostObject->GetGhostID(), stateMask);
            }
        }
    }
    else if(headerSizeInBits + packetSizeInBits + packingInfo.m_CachedObjectStream.GetBitLength() <= MAX_PACKET_BIT_SIZE)
    {
        // jason todo: check if the current cached object can go into the packet
        // if not, return false again
        countObjectsInPacket++;
        ghostObjectStream += packingInfo.m_CachedObjectStream;

        GhostTransmissionRecord& gtr = tr.m_GhostTransmissionRecords.emplace_back(packingInfo.m_CachedTransmissionRecord);
        
        if (GhostObject* ghostObject = packingInfo.m_ObjectsToPack.size() ? m_GhostObjectsIDMap[packingInfo.m_ObjectsToPack[packingInfo.m_LastPackedIndex].first] : nullptr)
        {
#ifdef CLIENT
            if (GhostTransmissionRecord* latestGhostTransmissionRecord = ghostObject->GetLatestTransmissionRecord())
#else
            if (GhostTransmissionRecord* latestGhostTransmissionRecord = ghostObject->GetLatestTransmissionRecord(tr.m_TargetPeerID))
#endif
            {
                gtr.m_PreviousTransmissionRecord = latestGhostTransmissionRecord;
                latestGhostTransmissionRecord->m_NextTransmissionRecord = &tr.m_GhostTransmissionRecords.back();
            }

#ifdef CLIENT
            ghostObject->SetLatestTransmissionRecord(&gtr);
#else
            ghostObject->SetLatestTransmissionRecord(tr.m_TargetPeerID, &gtr);
#endif
        }
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
    for (size_t i = packingInfo.m_LastPackedIndex; i < packingInfo.m_ObjectsToPack.size(); ++i)
    {
        // Packet structure [GhostID][StatusChange][StateMask][StateData]
        packingInfo.m_CachedObjectStream.Clear();

        auto& [ghostID, stateMask] = packingInfo.m_ObjectsToPack[i];

        GhostObject* ghostObject = m_GhostObjectsIDMap[ghostID];

        // Reminder here that the ghost id here has the peer id packed as the first 3 bits
        // so in the receiving end, we can >> 13 to get the peer id
        packingInfo.m_CachedObjectStream << ghostID;

        // Check if there is a status change; need to send create/destroy status change
        GhostStatusType status = GhostStatusType::None;
        if (packingInfo.m_GhostsToCreate.find(ghostID) != packingInfo.m_GhostsToCreate.end())
        {
            // Object is flagged for creation
            packingInfo.m_CachedObjectStream << true << true;
            status = GhostStatusType::Create;

            // jason todo: here we should call the custom function from scene to pack object creation stuff into the packet
#ifdef CLIENT
            ghostObject->ReplicateGhostObjectToBitstream(packingInfo.m_CachedObjectStream);
#else
            ghostObject->ReplicateGhostObjectToBitstream(tr.m_TargetPeerID, packingInfo.m_CachedObjectStream);
#endif
        }
        else if (packingInfo.m_GhostsToDelete.find(ghostID) != packingInfo.m_GhostsToDelete.end())
        {
            // Object is flagged for removal, do not need to pack state data
            packingInfo.m_CachedObjectStream << true << false;
            status = GhostStatusType::Delete;
        }
        else
        {
            // No status change, send states as per normal
            packingInfo.m_CachedObjectStream << false;
            packingInfo.m_CachedObjectStream << stateMask;
#ifdef CLIENT
            ghostObject->WriteStream(packingInfo.m_CachedObjectStream, stateMask);
#else
            ghostObject->WriteStream(tr.m_TargetPeerID, packingInfo.m_CachedObjectStream, stateMask);
#endif
        }

        packingInfo.m_CachedTransmissionRecord.m_GhostID = ghostObject->GetGhostID();
        packingInfo.m_CachedTransmissionRecord.m_StateMask = stateMask;
        packingInfo.m_CachedTransmissionRecord.m_Status = status;

        // Checking for buffer overflow...
        // HeaderSize + ObjectsStreamSize + CurrentObjectSize + CurrentPacketSize > MaxPacketBitSize
        if (headerSizeInBits + ghostObjectStream.GetBitLength() + packingInfo.m_CachedObjectStream.GetBitLength() + packetSizeInBits <= MAX_PACKET_BIT_SIZE)
        {
            countObjectsInPacket++;
            packingInfo.m_LastPackedIndex++;
            ghostObjectStream += packingInfo.m_CachedObjectStream;

            // Create new transmission record for object
            GhostTransmissionRecord& gtr = tr.m_GhostTransmissionRecords.emplace_back(packingInfo.m_CachedTransmissionRecord);

#ifdef CLIENT
            if (GhostTransmissionRecord* latestGhostTransmissionRecord = ghostObject->GetLatestTransmissionRecord())
#else
            if (GhostTransmissionRecord* latestGhostTransmissionRecord = ghostObject->GetLatestTransmissionRecord(tr.m_TargetPeerID))
#endif
            {
                gtr.m_PreviousTransmissionRecord = latestGhostTransmissionRecord;
                latestGhostTransmissionRecord->m_NextTransmissionRecord = &tr.m_GhostTransmissionRecords.back();
            }

#ifdef CLIENT
            ghostObject->SetLatestTransmissionRecord(&gtr);
#else
            ghostObject->SetLatestTransmissionRecord(tr.m_TargetPeerID, &gtr);
#endif
        } 
        else
        { 
            // Packet what ever we have and notify that we still have more ghost objects to pack
            packet.m_GhostStream << countObjectsInPacket;
            packet.m_GhostStream += ghostObjectStream;
            return false;
        }
    }

    // Successfully packed everthing into the current packet
    packet.m_GhostStream << countObjectsInPacket;
    packet.m_GhostStream += ghostObjectStream;
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
#ifdef CLIENT
    PackingInfo& packingInfo = m_PackingInfo;
#else
    PackingInfo& packingInfo = m_PackingInfo[tr.m_TargetPeerID];
#endif

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
            packingInfo.m_GhostsToCreate.insert(gtr.m_GhostID);
            continue;
        }
        else if (gtr.m_Status == GhostStatusType::Delete)
        {
            packingInfo.m_GhostsToDelete.insert(gtr.m_GhostID);
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
        ghostObject->SetRetransmissionMask(tr.m_TargetPeerID, gtr.m_StateMask);
#endif
    }
}

void GhostManager::SyncAllObjectProperties()
{
    for (GhostObject* ghostObject : m_GhostObjects)
    {
        ghostObject->SyncProperties();
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
    for (auto& [peerID, packingInfo] : m_PackingInfo)
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
#ifdef CLIENT
    m_PackingInfo.m_GhostsToCreate.clear();
    m_PackingInfo.m_GhostsToDelete.clear();
#else
    for (auto& [peerID, packingInfo] : m_PackingInfo)
    {
        packingInfo.m_GhostsToCreate.clear();
        packingInfo.m_GhostsToDelete.clear();
    }
#endif
}

void GhostManager::RegisterGhostObject(GhostObject* ghostObject)
{   
    m_GhostObjects.push_back(ghostObject);
    m_GhostObjectsIDMap.try_emplace(ghostObject->GetGhostID(), ghostObject);
}

void GhostManager::UnregisterGhostObject(GhostObject* ghostObject)
{
    // Check the first 3 bits for matching peer id. if id is from this peer then release back into the pool
    if (ghostObject->GetGhostID() >> 12 == StreamManager::GetInstance().GetPeerID())
    {
        m_AvailableGhostIDs.push(ghostObject->GetGhostID());
    }

    m_GhostObjects.erase(std::remove(m_GhostObjects.begin(), m_GhostObjects.end(), ghostObject));
    m_GhostObjectsIDMap.erase(ghostObject->GetGhostID());
}

#ifdef CLIENT

void GhostManager::ReplicateToServer(GhostID ghostID)
{
    m_PackingInfo.m_GhostsToCreate.emplace(ghostID);
}

#else

void GhostManager::CreatePeer(PeerID peerID)
{
    m_PackingInfo.try_emplace(peerID, PackingInfo{});
}

void GhostManager::ReplicateForAllPeer(GhostID ghostID)
{
    for (auto& [peerID, packingInfo] : m_PackingInfo)
    {
        packingInfo.m_GhostsToCreate.emplace(ghostID);
    }
}

void GhostManager::UnreplicateForAllPeer(GhostID ghostID)
{
    for (auto& [peerID, packingInfo] : m_PackingInfo)
    {
        packingInfo.m_GhostsToDelete.emplace(ghostID);
    }
}

void GhostManager::ReplicateForPeer(PeerID targetPeerID, GhostID ghostID)
{
    m_PackingInfo[targetPeerID].m_GhostsToCreate.emplace(ghostID);
}

void GhostManager::UnreplicateForPeer(PeerID targetPeerID, GhostID ghostID)
{
    m_PackingInfo[targetPeerID].m_GhostsToDelete.emplace(ghostID);
}

#endif
