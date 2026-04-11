#include "pch.h"
#include "CColliderPoint.h"

#include "CColliderCircle.h"
#include "CColliderRect.h"

CColliderPoint::CColliderPoint()
    : CCollider2D(COMPONENT_TYPE::COLLIDER2D_POINT)
{
}

CColliderPoint::~CColliderPoint()
{
}

void CColliderPoint::FinalTick()
{
    if (!GetMovable() && IsCacheValid())
    {
        DrawDebugCircle(GetWorldPos(), 3.f, GetColor(), 0.f);
        return;
    }

    CCollider2D::FinalTick();
    
    const Matrix matTranslation = XMMatrixTranslation(GetOffset().x, GetOffset().y, 0.f);
    GetWorldMat() = matTranslation * Transform()->GetWorldMatrix();
    
    DrawDebugCircle(GetWorldPos(), 3.f, GetColor(), 0.f);
}

bool CColliderPoint::IsCollision(CColliderRect* _OtherRect)
{
    return _OtherRect->IsCollision(this);
}

bool CColliderPoint::IsCollision(CColliderPoint* _OtherPoint)
{
    // 2D 위치가 같다면 같다고 판단처리 (딱히 의미는 없음, 점과 점 성분이 같으면 같다고 판단)
    const Vec3 ThisPos  = GetWorldPos();
    const Vec3 OtherPos = _OtherPoint->GetWorldPos();
    
    return ThisPos.x == OtherPos.x && ThisPos.y == OtherPos.y;
}

bool CColliderPoint::IsCollision(CColliderCircle* _OtherCircle)
{
    return _OtherCircle->IsCollision(this);    
}
