#pragma once

#include "singleton.h"
#include <unordered_map>
#include "ghost.h"

struct GhostRecord;

class ServerGhostManager
{
public:
  void MarkGhostForTransfer(Ghostable* obj);
  void MarkGhostForDestroy(Ghostable* obj);

  void Pack(Bitstream* bitstream);

private:
  uint32_t GetNextGhostId();

  std::vector<Ghostable*> m_Ghosted;
  uint32_t                m_GhostIdCounter;
};

// The goal of the remote ghost manager is to handle the logic of ghost objects
// on the remote client itself
class RemoteGhostManager : public Singleton<RemoteGhostManager>
{
public:
  
private:
  void InitializeInternal() override;
  void ShutdownInternal() override;
};
