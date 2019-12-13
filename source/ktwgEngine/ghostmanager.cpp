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
        m_AvailableGhostIDs.push((static_cast<GhostID>(peerID) << 12) | i);
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
        else
        {
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
                // assert(false);
            }
        }

        countObjectsInPacket--;
    }
}

bool GhostManager::WritePacket(Packet& packet, TransmissionRecord& tr)
{
#ifdef CLIENT
    PackingInfo& packingInfo = m_PackingInfo;
#else
    PackingInfo& packingInfo = m_PackingInfo[tr.m_TargetPeerID];
#endif

    // Rebuild packing list for a fresh update
    if (packingInfo.m_IsDonePacking)
    {
        packingInfo.m_IsDonePacking = false;
        packingInfo.m_PackedStreams.clear();
        packingInfo.m_PackedIndexCache = 0;

        for (GhostObject* ghostObject : m_GhostObjects)
        {
            auto& [gtr, stream] = packingInfo.m_PackedStreams.emplace_back();
            
            if (!PackGhostObject(ghostObject->GetGhostID(), tr.m_TargetPeerID, stream, gtr))
            {
                packingInfo.m_PackedStreams.pop_back();
            }
        }

        // Nothing to pack, return packing success
        if (packingInfo.m_PackedStreams.empty()) 
        {
            return true;
        }
    }

    uint16_t countObjectsInPacket = 0;
    const size_t headerSizeInBits = sizeof(countObjectsInPacket) * 8;
    const size_t packetSizeInBits = packet.GetStreamSize();

    BitStream ghostObjectStream; // This one only has the actual objects in the stream

    while (packingInfo.m_PackedIndexCache < packingInfo.m_PackedStreams.size())
    {
        auto&[gtrTmp, stream] = packingInfo.m_PackedStreams[packingInfo.m_PackedIndexCache];

        if (headerSizeInBits + packetSizeInBits + stream.GetBitPosition() <= MAX_PACKET_BIT_SIZE)
        {
            packingInfo.m_PackedIndexCache++;
            ghostObjectStream += stream;
            countObjectsInPacket++;
        }
        else
        {
            if (countObjectsInPacket > 0)
            {
                packet.m_GhostStream << countObjectsInPacket;
                packet.m_GhostStream += ghostObjectStream;
            }
            return false;
        }

        GhostTransmissionRecord& gtr = tr.m_GhostTransmissionRecords.emplace_back(gtrTmp);

        if (GhostObject* ghostObject = m_GhostObjectsIDMap[gtr.m_GhostID])
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

    packet.m_GhostStream << countObjectsInPacket;
    packet.m_GhostStream += ghostObjectStream;
    packingInfo.m_IsDonePacking = true;
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
    for (auto&[peerID, packingInfo] : m_PackingInfo)
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
    for (auto&[peerID, packingInfo] : m_PackingInfo)
    {
        packingInfo.m_GhostsToCreate.clear();
        packingInfo.m_GhostsToDelete.clear();
    }
#endif
}

void GhostManager::RegisterGhostID(GhostObject * ghostObject)
{
    m_GhostObjectsIDMap.try_emplace(ghostObject->GetGhostID(), ghostObject);
}

void GhostManager::RegisterGhostObject(GhostObject* ghostObject)
{
    m_GhostObjects.push_back(ghostObject);
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
    for (auto&[peerID, packingInfo] : m_PackingInfo)
    {
        packingInfo.m_GhostsToCreate.emplace(ghostID);
    }
}

void GhostManager::UnreplicateForAllPeer(GhostID ghostID)
{
    for (auto&[peerID, packingInfo] : m_PackingInfo)
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

void GhostManager::ReplicateGhostObjectsToPeer(PeerID peerID)
{
  for (auto& ghostObject : m_GhostObjects)
  {
    ReplicateForPeer(peerID, ghostObject->GetGhostID());
  }
}

void GhostManager::UnreplicateGhostObjectsToPeer(PeerID peerID)
{
  for (auto& ghostObject : m_GhostObjects)
  {
    UnreplicateForPeer(peerID, ghostObject->GetGhostID());
  }
}

void GhostManager::ReplicateAllGhostsToAllPeers()
{
  for (auto& ghostObject : m_GhostObjects)
  {
    ReplicateForAllPeer(ghostObject->GetGhostID());
  }
}

void GhostManager::UnreplicateAllGhostsToAllPeers()
{
  for (auto& ghostObject : m_GhostObjects)
  {
    UnreplicateForAllPeer(ghostObject->GetGhostID());
  }
}

bool GhostManager::PackGhostObject(const GhostID ghostID, const PeerID peerID, BitStream & stream, GhostTransmissionRecord& gtr)
{
    GhostObject* ghostObject = m_GhostObjectsIDMap[ghostID];
    if (ghostObject == nullptr)
    {
        return false;
    }

#ifdef CLIENT
    PackingInfo& packingInfo = m_PackingInfo;
#else
    PackingInfo& packingInfo = m_PackingInfo[peerID];
#endif
    
    // Reminder here that the ghost id here has the peer id packed as the first 3 bits
    // so in the receiving end, we can >> 13 to get the peer id
    gtr.m_GhostID = ghostID;
    stream << ghostID;

    // Check if there is a status change; need to send create/destroy status change
    GhostStatusType status = GhostStatusType::None;

    if (packingInfo.m_GhostsToCreate.find(ghostID) != packingInfo.m_GhostsToCreate.end())
    {
        // Object is flagged for creation
        stream << true << true;
        packingInfo.m_GhostsToCreate.erase(ghostID);
        status = GhostStatusType::Create;

#ifdef CLIENT
        ghostObject->ReplicateGhostObjectToBitstream(stream);
#else
        ghostObject->ReplicateGhostObjectToBitstream(peerID, stream);
#endif
    }
    else if (packingInfo.m_GhostsToDelete.find(ghostID) != packingInfo.m_GhostsToDelete.end())
    {
        // Object is flagged for removal, do not need to pack state data
        stream << true << false;
        packingInfo.m_GhostsToDelete.erase(ghostID);
        status = GhostStatusType::Delete;
    }
    else
    {
        bool needUpdate = false;
#ifdef CLIENT
        gtr.m_StateMask = = ghostObject->GetStateMaskAndCheckNeedUpdate(needUpdate);
#else
        gtr.m_StateMask = ghostObject->GetStateMaskAndCheckNeedUpdate(peerID, needUpdate);
#endif
        if (needUpdate == false)
        {
            return false;
        }
        // No status change, send states as per normal
        packingInfo.m_CachedObjectStream << false;
        packingInfo.m_CachedObjectStream << gtr.m_StateMask;
#ifdef CLIENT
        ghostObject->WriteStream(stream, gtr.m_StateMask);
#else
        ghostObject->WriteStream(peerID, stream, gtr.m_StateMask);
#endif
    }
}

#endif
