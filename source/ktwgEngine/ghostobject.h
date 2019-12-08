#pragma once
#include <stdint.h>
#include "ghostproperty.h"
#include "ghostobjectids.h"
#include "netdefs.h"
#include <vector>
#include <type_traits>

using GhostID = unsigned short;

struct GhostTransmissionRecord;

class GhostObject
{
public:

    GhostObject();
    virtual ~GhostObject();
    
    GhostStateMask GetFullStateMask() const;
    void SetGhostID(GhostID ghostID) { m_GhostID = ghostID; }
    GhostID GetGhostID() const { return m_GhostID; }
    void SetPeerID(PeerID peerID) { m_PeerID = peerID; }
    size_t GetPropertyCount() const { return m_GhostProperties.size(); }

#ifdef CLIENT
    GhostStateMask GetStateMaskAndCheckNeedUpdate(bool& outNeedUpdate);
    void SetRetransmissionMask(const GhostStateMask& stateMask);
    GhostTransmissionRecord* GetLatestTransmissionRecord() { return m_LatestTransmissionRecord; }
    void SetLatestTransmissionRecord(GhostTransmissionRecord* transmissionRecord) { m_LatestTransmissionRecord = transmissionRecord; }
    void WriteStream(BitStream& stream, const GhostStateMask& stateMask);
#else
    GhostStateMask GetStateMaskAndCheckNeedUpdate(const PeerID targetPeerID, bool& outNeedUpdate);
    void SetRetransmissionMask(const PeerID targetPeerID, const GhostStateMask& stateMask);
    GhostTransmissionRecord* GetLatestTransmissionRecord(const PeerID targetPeerID) { return m_LatestTransmissionRecord[targetPeerID]; }
    void SetLatestTransmissionRecord(const PeerID targetPeerID, GhostTransmissionRecord* transmissionRecord) { m_LatestTransmissionRecord[targetPeerID] = transmissionRecord; }
    void WriteStream(const PeerID targetPeerID, BitStream& stream, const GhostStateMask& stateMask);
#endif

    void ReadStream(BitStream& stream, const GhostStateMask& stateMask);
    void SyncProperties();
    bool IsOwner();

    template<typename T>
    void RegisterPropertyPOD(T& property, NetAuthority authority, size_t bitCount = sizeof(T) * 8)
    {
        m_GhostProperties.push_back(new GhostPropertyPOD<T>(property, authority, bitCount));

#ifdef CLIENT
        m_StatesToRetransmist.push_back(false);
#else
        m_StatesToBroadcast.push_back(false);
        for (auto& [peerID, stateMask] : m_StatesToRetransmit)
        {
            stateMask.push_back(false);
        }
#endif
    }

    void RegisterPropertyCustom(GhostProperty* ghostProperty)
    {
        m_GhostProperties.push_back(ghostProperty);
    }

private:

    PeerID m_PeerID;
    GhostID m_GhostID;
    std::vector<GhostProperty*> m_GhostProperties;
    
#ifdef CLIENT
    GhostStateMask m_StatesToRetransmit;
    GhostTransmissionRecord* m_LatestTransmissionRecord;
#else
    GhostStateMask m_StatesToBroadcast;
    std::map<PeerID, GhostStateMask> m_StatesToRetransmit;
    std::map<PeerID, GhostTransmissionRecord*> m_LatestTransmissionRecord;
#endif
};