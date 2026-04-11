#include "pch.h"
#include "CColliderCircle.h"

#include "CColliderPoint.h"
#include "CColliderRect.h"

CColliderCircle::CColliderCircle()
    : CCollider2D(COMPONENT_TYPE::COLLIDER2D_CIRCLE)
{
}

CColliderCircle::~CColliderCircle()
{
}

void CColliderCircle::FinalTick()
{
    if (!GetMovable() && IsCacheValid())
    {
        DrawDebugCircle(GetWorldPos(), m_Radius, GetColor(), 0.f);
        return;
    }

    CCollider2D::FinalTick();

    const Matrix matTranslation = XMMatrixTranslation(GetOffset().x, GetOffset().y, 0.f);
    GetWorldMat() = matTranslation * Transform()->GetWorldMatrix();
    
    DrawDebugCircle(GetWorldPos(), m_Radius, GetColor(), 0.f);
}

bool CColliderCircle::IsCollision(CColliderRect* _OtherRect)
{
    return _OtherRect->IsCollision(this);
}

bool CColliderCircle::IsCollision(const Vec3& _Point)
{
    Vec3 ThisPos = GetWorldPos(); ThisPos.z = 0.f;
    return Vec3::DistanceSquared(ThisPos, _Point) < m_Radius * m_Radius;
}

bool CColliderCircle::IsCollision(CColliderPoint* _OtherPoint)
{
    // ThisCircle vs Point
    
    Vec3 PointPos   = _OtherPoint->GetWorldPos(); PointPos.z = 0.f;
    Vec3 ThisPos    = GetWorldPos(); ThisPos.z = 0.f;
    
    float Temp = Vec3::DistanceSquared(PointPos, ThisPos); 
    
    return Vec3::DistanceSquared(PointPos, ThisPos) < m_Radius * m_Radius;
}

bool CColliderCircle::IsCollision(CColliderCircle* _OtherCircle)
{
    // ThisCircle vs OtherCircle
    
    Vec3 ThisPos    = GetWorldPos(); ThisPos.z = 0.f;
    Vec3 OtherPos   = _OtherCircle->GetWorldPos(); OtherPos.z = 0.f;
    
    const float RadiusSum = m_Radius + _OtherCircle->m_Radius;  
    
    return Vec3::DistanceSquared(ThisPos, OtherPos) < RadiusSum * RadiusSum;
}

void CColliderCircle::SaveToLevelFile(FILE* _File)
{
    CCollider2D::SaveToLevelFile(_File);
    
    fwrite(&m_Radius, sizeof(float), 1, _File);
}

void CColliderCircle::LoadFromLevelFile(FILE* _File)
{
    CCollider2D::LoadFromLevelFile(_File);
    fread(&m_Radius, sizeof(float), 1, _File);
}
