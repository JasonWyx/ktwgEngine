#pragma once
#include <unordered_map>
#include <iostream>

class PersistentObject;

using ghostId_t    = unsigned;
using ghostState_t = unsigned char;
using Bitstream    = std::stringstream;

enum class GhostAttribute : unsigned int
{
  POSITION = 0, // 0
  ROTATION // 1
};

enum class GhostStatus : unsigned char
{
  CLEAR,
  TRANSFERRING, // transferring to remote host
  DELETING // Deleting from remote host
};

struct GhostState
{
  ghostState_t m_Position : 1;
  ghostState_t m_Rotation : 1;
};

struct GhostRecord
{
  ghostId_t m_Id;
  GhostState m_StateMask;
  GhostStatus m_Status;
};

class Ghostable
{
public:
  void SetAlwaysGhost(bool alwaysGhost);
  bool GetAlwaysGhost() const;
  
  void SetGhostRecord(const GhostRecord& ghostRecord);
  const GhostRecord& GetGhostRecord() const;

private:
  GhostRecord m_GhostRecord;
  bool m_AlwaysGhost;
};

// Ghost is a persistent object (Should Inherit from persistent object)
class Ghost
{
public:
  void SetAttribute(GhostAttribute attrId, PersistentObject* attr);
  PersistentObject* GetAttribute(GhostAttribute attrId) const;

  // TODO: Make virtual overriding function
  void Unpack(Bitstream* stream);
  void Pack(Bitstream* stream);
  
private:
  std::unordered_map<GhostAttribute, PersistentObject*> m_Attributes;
};
