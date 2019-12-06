#pragma once
#include <stdint.h>
#include "ghostproperty.h"
#include "ghostobjectids.h"
#include <vector>
#include <type_traits>
#include "netdefs.h"

using GhostID = unsigned int;

struct GhostTransmissionRecord;

class GhostObject
{
public:

    GhostObject(GhostID ghostNetID, PeerID owner);
    virtual ~GhostObject();
    
    GhostStateMask GetFullStateMask() const;
    GhostID GetGhostID() const { return m_GhostID; }
    size_t GetPropertyCount() const { return m_GhostProperties.size(); }

#ifdef CLIENT
    GhostStateMask GetStateMaskAndCheckNeedUpdate(bool& outNeedUpdate);
    void SetRetransmissionMask(const GhostStateMask& stateMask);
    TransmissionRecord* GetLatestTransmissionRecord() { return m_LatestTransmissionRecord; }
    void SetLatestTransmissionRecord(TransmissionRecord* transmissionRecord) { m_LatestTransmissionRecord = transmissionRecord; }
    void WriteStream(BitStream& stream, const GhostStateMask& stateMask);
#else
    GhostStateMask GetStateMaskAndCheckNeedUpdate(const PeerID targetPeerID, bool& outNeedUpdate);
    void SetRetransmissionMask(const PeerID targetPeerID, const GhostStateMask& stateMask);
    TransmissionRecord* GetLatestTransmissionRecord(const PeerID targetPeerID) { return m_LatestTransmissionRecord[targetPeerID]; }
    void SetLatestTransmissionRecord(const PeerID targetPeerID, TransmissionRecord* transmissionRecord) { m_LatestTransmissionRecord[targetPeerID] = transmissionRecord; }
    void WriteStream(const PeerID targetPeerID, BitStream& stream, const GhostStateMask& stateMask);
#endif

    void ReadStream(BitStream& stream, const GhostStateMask& stateMask);
    void SyncPropertyValues();
    inline bool IsOwner();

    template<typename T>
    void RegisterGhostProperty(T& property, NetAuthority authority, size_t bitCount = sizeof(T) * 8)
    {
        m_GhostProperties.push_back(new GhostPropertyVirtual<T>(property, authority, bitCount));
    }

private:

    PeerID m_Owner;
    GhostID m_GhostID;
    std::vector<GhostProperty*> m_GhostProperties;
    
#ifdef CLIENT
    GhostStateMask m_StatesToRetransmit;
    TransmissionRecord* m_LatestTransmissionRecord;
#else
    GhostStateMask m_StatesToBroadcast;
    std::map<PeerID, GhostStateMask> m_StatesToRetransmit;
    std::map<PeerID, TransmissionRecord*> m_LatestTransmissionRecord;
#endif
};