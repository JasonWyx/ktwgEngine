#include "ghostmanager.h"

void ServerGhostManager::MarkGhostForTransfer(Ghostable* obj)
{
  GhostRecord record;
  record.m_Id = GetNextGhostId();
  record.m_StateMask = {};
  record.m_Status = GhostStatus::TRANSFERRING;
  obj->SetGhostRecord(record);

  m_Ghosted.emplace_back(obj);
}

void ServerGhostManager::MarkGhostForDestroy(Ghostable* obj)
{
  GhostRecord record = obj->GetGhostRecord();
  record.m_Status = GhostStatus::DELETING;
  obj->SetGhostRecord(record);
}

void ServerGhostManager::Pack(Bitstream * bitstream)
{
}

uint32_t ServerGhostManager::GetNextGhostId()
{
  return m_GhostIdCounter++;
}
