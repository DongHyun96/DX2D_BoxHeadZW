#include "pch.h"
#include "Entity.h"

UINT Entity::g_NextID{};

Entity::Entity()
    : m_InstID(g_NextID++)
    , m_Name()
    , m_RefCount(0)
    
{
}

Entity::Entity(const Entity& _Other)
    : m_InstID(++g_NextID)
    , m_Name(_Other.m_Name)
    , m_RefCount(0)
{
}

Entity::~Entity()
{
    // --g_NextID;
}
