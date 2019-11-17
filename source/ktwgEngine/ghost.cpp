#include "ghost.h"

void Ghostable::SetGhostRecord(const GhostRecord& ghostRecord)
{
  m_GhostRecord = ghostRecord;
}
  
const GhostRecord& Ghostable::GetGhostRecord() const
{
  return m_GhostRecord;
}

void Ghostable::SetAlwaysGhost(bool alwaysGhost)
{
  m_AlwaysGhost = alwaysGhost;
}

bool Ghostable::GetAlwaysGhost() const
{
  return m_AlwaysGhost;
}

void Ghost::SetAttribute(GhostAttribute attrId, PersistentObject* attr)
{
  m_Attributes[attrId] = attr;
}

PersistentObject* Ghost::GetAttribute(GhostAttribute attrId) const
{
  try
  {
    return m_Attributes.at(attrId);
  }
  catch (const std::out_of_range& e)
  {
    return nullptr;
  }
}

void Ghost::Unpack(Bitstream* stream)
{
}

void Ghost::Pack(Bitstream* stream)
{
}
