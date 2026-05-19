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

Entity::Entity(Entity&& _Other) noexcept
    : m_InstID(g_NextID++)
    , m_RefCount(0)
    , m_Name(move(_Other.m_Name))
{
}

Entity& Entity::operator=(const Entity& _Other)
{
    if (this == &_Other) return *this;

    // m_InstID는 덮어쓸수도 없고, 애초에 덮어써서도 안됨
    // m_RefCount는 현재 나를 가리키는 Ptr들의 갯수이므로 절대 건드려서는 안됨
    // Name만 복사대입처리
    
    m_Name = _Other.m_Name;
    
    return *this;
}

Entity& Entity::operator=(Entity&& _Other) noexcept
{
    if (this == &_Other) return *this;

    // 복사 대입과 마찬가지로 고유 식별자와 참조 카운트는 무시
    m_Name = move(_Other.m_Name);
    
    return *this;
}

Entity::~Entity()
{
    // --g_NextID;
}
