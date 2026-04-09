#include "pch.h"
#include "CCollider2D.h"

#include "CColliderCircle.h"
#include "CColliderPoint.h"
#include "CColliderRect.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"


CCollider2D::CCollider2D(COMPONENT_TYPE _Type)
    : Component(_Type)
    , m_OverlapCount(0)
{
}

CCollider2D::CCollider2D(const CCollider2D& _Origin)
    : Component(_Origin)
    , m_Offset(_Origin.m_Offset)
    , m_Color(_Origin.m_Color)
    , m_bIsActive(_Origin.m_bIsActive)
    , m_OverlapCount(0)
    , m_vecBeginDelegates{}
    , m_vecOverlapDelegates{}
    , m_vecEndDelegates{}
{
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::FinalTick()
{
    m_Color = (m_OverlapCount > 0) ? Vec4(1.f, 0.f, 0.f, 1.f) : Vec4(0.f, 1.f, 0.f, 1.f);
    assert(m_OverlapCount >= 0);
}

void CCollider2D::BeginOverlap(const Ptr<CCollider2D>& _Other)
{
    ++m_OverlapCount;

    for (int i = 0; i < m_vecBeginDelegates.size(); ++i)
        (m_vecBeginDelegates[i].Inst->*m_vecBeginDelegates[i].MemFunc)(this, _Other.Get());
}

void CCollider2D::Overlap(const Ptr<CCollider2D>& _Other)
{
    for (int i = 0; i < m_vecOverlapDelegates.size(); ++i)
        (m_vecOverlapDelegates[i].Inst->*m_vecOverlapDelegates[i].MemFunc)(this, _Other.Get());
}

void CCollider2D::EndOverlap(const Ptr<CCollider2D>& _Other)
{
    --m_OverlapCount;
    
    for (int i = 0; i < m_vecEndDelegates.size(); ++i)
        (m_vecEndDelegates[i].Inst->*m_vecEndDelegates[i].MemFunc)(this, _Other.Get());
}







void CCollider2D::AddDynamicBeginOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc)
{
    m_vecBeginDelegates.push_back(COLLISION_DELEGATE(_Inst, _MemFunc));
}

void CCollider2D::AddDynamicOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc)
{
    m_vecOverlapDelegates.push_back(COLLISION_DELEGATE(_Inst, _MemFunc));
}

void CCollider2D::AddDynamicEndOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc)
{
    m_vecEndDelegates.push_back(COLLISION_DELEGATE(_Inst, _MemFunc));
}

bool CCollider2D::IsCollision(const Ptr<CCollider2D>& _Other)
{
    if (!this->GetOwner()->GetActive() || !_Other->GetOwner()->GetActive()) return false;                // Active가 꺼진 오브젝트
    if (this->GetOwner()->IsObjectDestroyed() || _Other->GetOwner()->IsObjectDestroyed()) return false;  // 곧 삭제 처리될 오브젝트
    if (!this->GetActive() || !_Other->GetActive()) return false; // Collision Component가 꺼진 상태

    switch (_Other->GetComponentType())
    {
    case COMPONENT_TYPE::COLLIDER2D_RECT:
        return IsCollision(static_cast<CColliderRect*>(_Other.Get()));
    case COMPONENT_TYPE::COLLIDER2D_POINT:
        return IsCollision(static_cast<CColliderPoint*>(_Other.Get()));
    case COMPONENT_TYPE::COLLIDER2D_CIRCLE:
        return IsCollision(static_cast<CColliderCircle*>(_Other.Get()));
    default:
        break;
    }

    assert(nullptr);
    return false;
}

void CCollider2D::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_Offset, sizeof(Vec3), 1, _File);
    fwrite(&m_Color, sizeof(Vec4), 1, _File);
    fwrite(&m_bIsActive, sizeof(bool), 1, _File);
    
}

void CCollider2D::LoadFromLevelFile(FILE* _File)
{
    fread(&m_Offset, sizeof(Vec3), 1, _File);
    fread(&m_Color, sizeof(Vec4), 1, _File);
    fread(&m_bIsActive, sizeof(bool), 1, _File);
}
