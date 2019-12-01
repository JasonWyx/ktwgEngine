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

bool GhostManager::ReadStream(BitStream& bitstream)
{
    uint16_t numObjectsInPacket = 0;
    bitstream >> numObjectsInPacket;

    while (numObjectsInPacket > 0)
    {
        bool isStatusChanged = false;
        bool isCreateNew = false;

        bitstream >> isStatusChanged;
        if (isStatusChanged)
        {
            bitstream >> isCreateNew;
            if (isCreateNew)
            {
                // Create new object with class id, with server as the authority
                uint32_t ghostClassID = 0;
                bitstream.Read(ghostClassID, 0u);
                // jason todo: what should be here to actually create the object?
            }
            else
            {
                // Flag object for removal
            }
        }

        GhostID ghostNetID = 0;
        bitstream >> ghostNetID;

        GhostObject* ghostObject = nullptr;
        auto iter = m_GhostObjectsIDMap.find(ghostNetID);
        if (iter != m_GhostObjectsIDMap.end())
        {
            // Here we process the packet straight away, but we might already have the latest data?
            // Nevermind this should already be the latest data
            ghostObject = iter->second;
            GhostStateMask stateMask(ghostObject->GetPropertyCount());
            bitstream >> stateMask;
            ghostObject->ReadStream(bitstream, stateMask);
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

bool GhostManager::WriteStream(BitStream& packetStream, TransmissionRecord& tr)
{
    uint16_t numObjectsInPacket = 0;
    constexpr size_t headerBitCount = sizeof(numObjectsInPacket) * 8;
    const size_t currentPacketBitCount = packetStream.GetBitLength();

    BitStream ghostPacketStream; // This one includes the number of ghost objects in the packet stream
    BitStream streamWithObjects;

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
    else
    {
        // jason todo: check if the current cached object can go into the packet
        // if not, return false again
    }

    for (size_t i = m_LastPackedIndex; i < m_ObjectsToPack.size(); ++i)
    {
        m_CachedObjectStream.Clear();

        auto& [ghostObject, stateMask] = m_ObjectsToPack[i];

        // Check if there is a status change; need to send create/destroy status change
        GhostStatusType status = GhostStatusType::None;
        if (m_GhostsToCreate.find(ghostObject->GetGhostID()) != m_GhostsToCreate.end())
        {
            m_CachedObjectStream << true << true;
            m_CachedObjectStream.Write(0u, 4); // jason todo: replace 0u with ghost class id
            status = GhostStatusType::Create;
        }
        else if (m_GhostsToDelete.find(ghostObject->GetGhostID()) != m_GhostsToDelete.end())
        {
            m_CachedObjectStream << true << false;
            status = GhostStatusType::Delete;
        }
        else
        {
            m_CachedObjectStream << false;
        }

        ghostObject->WriteStream(m_CachedObjectStream, stateMask);

        // Checking for buffer overflow...
        // HeaderSize + ObjectsStreamSize + CurrentObjectSize + CurrentPacketSize > MaxPacketBitSize
        if (headerBitCount + streamWithObjects.GetBitLength() + m_CachedObjectStream.GetBitLength() + currentPacketBitCount > MAX_PACKET_BIT_SIZE)
        {
            return false;
        }
        else
        {
            m_LastPackedIndex++;
            numObjectsInPacket++;
            streamWithObjects += m_CachedObjectStream;

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
    }

    packetStream << numObjectsInPacket;
    packetStream += streamWithObjects;
    packetStream += packetStream;

    return true;
}

void GhostManager::NotifyTransmissionSuccess(TransmissionRecord& tr)
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

void GhostManager::NotifyTransmissionFailure(TransmissionRecord& tr)
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

void GhostManager::DropPendingStreamData()
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
