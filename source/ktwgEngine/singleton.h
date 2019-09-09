#pragma once
#include <cassert>

/******************************************************************************/
/*
  Simple singleton wrapper class
*/
/******************************************************************************/
template<typename T>
class Singleton
{
public:

  Singleton()
  {
    // Ensures that the singleton is only instantiated through the GetInstancePtr() function
    assert(!ms_IsInitialized && ms_Instance == nullptr);
  }

  virtual ~Singleton()
  {
    // Ensure singleton's resources are properly released
    assert(ms_IsInitialized && ms_Instance != nullptr);
  }
  Singleton(const Singleton<T>&) = delete;
  Singleton(Singleton<T>&&) = delete;
  Singleton<T>& operator=(const Singleton<T>&) = delete;
  Singleton<T>& operator=(Singleton<T>&&) = delete;

  void InitializeImpl() { InitializeInternal(); }
  void ShutdownImpl()   { ShutdownInternal(); }

  static void Initialize() 
  {
    assert(!ms_IsInitialized);
    ms_Instance = new T();
    ms_Instance->InitializeImpl();
    ms_IsInitialized = true;
  }

  static void Shutdown()
  { 
    assert(ms_IsInitialized);
    ms_Instance->ShutdownImpl();
    delete  ms_Instance;
    ms_IsInitialized = false;
  }

  static T* GetInstancePtr()
  {
    assert(ms_IsInitialized);
    return ms_Instance;
  }

  static T& GetInstance() { return *GetInstancePtr(); }

  static bool IsInitialized() { return ms_IsInitialized; }

protected:

  virtual void InitializeInternal() = 0;
  virtual void ShutdownInternal() = 0;

private:

  inline static T* ms_Instance = nullptr;
  inline static bool ms_IsInitialized = false;
};