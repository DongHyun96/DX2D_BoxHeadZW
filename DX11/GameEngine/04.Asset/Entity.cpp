#include "pch.h"
#include "Entity.h"

UINT Entity::g_NextID{};

Entity::Entity()
    : m_InstID(g_NextID++)
    , m_RefCount(0)
    , m_Name()
    
{
}

Entity::Entity(const Entity& _Other)
    : m_InstID(g_NextID++)
    , m_RefCount(0)
    , m_Name(_Other.m_Name)
{
}

Entity::~Entity()
{
    // --g_NextID;
}
