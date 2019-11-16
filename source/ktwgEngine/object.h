#pragma once
#include <string>
#include <numeric>

class Object
{
public:

    Object(uint32_t objectID);
    virtual ~Object();

    virtual void Update(float dt) { }

    uint32_t GetObjectID() const { return m_ObjectID; }

private:

    uint32_t m_ObjectID;
};