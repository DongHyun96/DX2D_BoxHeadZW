#include "pch.h"
#include "CColliderRect.h"

#include "CColliderCircle.h"
#include "CColliderPoint.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"


CColliderRect::CColliderRect()
    : CCollider2D(COMPONENT_TYPE::COLLIDER2D_RECT)
{
}

CColliderRect::~CColliderRect()
{
}

void CColliderRect::FinalTick()
{
    if (!GetMovable() && IsCacheValid())
    {
        DrawDebugRect(GetWorldMat(), GetColor(), 0.f);
        return;
    }

    CCollider2D::FinalTick();
    
    Matrix matPivotToOrigin = XMMatrixTranslation(-m_PivotLocal.x, -m_PivotLocal.y, 0.f);
    Matrix matScale         = XMMatrixScaling(m_Scale.x, m_Scale.y, 1.f);
    Matrix matPivotBack     = XMMatrixTranslation( m_PivotLocal.x,  m_PivotLocal.y, 0.f);
    Matrix matTranslation   = XMMatrixTranslation(GetOffset().x, GetOffset().y, 0.f);

    // row-vector 기준: T(-Pivot) * S * T(Pivot)
    GetWorldMat() = matPivotToOrigin * matScale * matPivotBack * matTranslation * Transform()->GetWorldMatrix();
    
    // 이 정보를 사용해서 Collision 계산을 행할 수 있을 듯? (두 번 계산처리되는 중)
    static const Vec3 kLocalCorners[4] = 
    {
        Vec3(-0.5f,  0.5f, 0.f), // LT
        Vec3( 0.5f,  0.5f, 0.f), // RT
        Vec3( 0.5f, -0.5f, 0.f), // RB
        Vec3(-0.5f, -0.5f, 0.f), // LB
    };
    
    for (int i = 0; i < 4; ++i)
        m_CornersWorldPos[i] = XMVector3TransformCoord(kLocalCorners[i], GetWorldMat());
    
    // Optimization: Pre-calculate normalized axes and half-lengths
    m_CachedRightAxis = m_CornersWorldPos[1] - m_CornersWorldPos[0];
    m_CachedUpAxis    = m_CornersWorldPos[3] - m_CornersWorldPos[0];
    m_CachedHalfW     = m_CachedRightAxis.Length() * 0.5f;
    m_CachedHalfH     = m_CachedUpAxis.Length() * 0.5f;
    m_CachedRightAxis.Normalize();
    m_CachedUpAxis.Normalize();

    // 각 변의 중점
    m_EdgeMid[0] = (m_CornersWorldPos[0] + m_CornersWorldPos[1]) * 0.5f; // Top
    m_EdgeMid[1] = (m_CornersWorldPos[1] + m_CornersWorldPos[2]) * 0.5f; // Right
    m_EdgeMid[2] = (m_CornersWorldPos[2] + m_CornersWorldPos[3]) * 0.5f; // Bottom
    m_EdgeMid[3] = (m_CornersWorldPos[3] + m_CornersWorldPos[0]) * 0.5f; // Left
    

    DrawDebugRect(GetWorldMat(), GetColor(), 0.f);
}

bool CColliderRect::IsCollision(CColliderRect* _OtherRect)
{
    // 두 사각형 모두 회전이 들어있지 않으면 AABBCollision 검사로 처리
    if (this->Transform()->GetDir(DIR::RIGHT) == Vec3::Right &&
        _OtherRect->Transform()->GetDir(DIR::RIGHT) == Vec3::Right)
        return AABBCollision(_OtherRect);
    
    return OBBCollision(_OtherRect);
}

bool CColliderRect::IsCollision(CColliderCircle* _OtherCircle)
{
    // this Rect의 회전이 들어가 있지 않다면 AABBCollision 검사로 처리
    return (this->Transform()->GetDir(DIR::RIGHT) == Vec3::Right) ? AABBCollision(_OtherCircle) : OBBCollision(_OtherCircle);
    // return OBBCollision(_OtherCircle);
}

bool CColliderRect::IsCollision(CColliderPoint* _OtherPoint)
{
    // this Rect의 회전이 들어가 있지 않다면 AABBCollision 검사로 처리
    return (this->Transform()->GetDir(DIR::RIGHT) == Vec3::Right) ? AABBCollision(_OtherPoint) : OBBCollision(_OtherPoint);
    // return OBBCollision(_OtherPoint);
}

bool CColliderRect::AABBCollision(CColliderRect* _OtherRect)
{
    // Rect vs Rect AABB
    
    // LT.x, RB.x, LT.y, RB.y 로 각 Rect의 AABB를 구성
    // m_CornersWorldPos : [0]=LT [1]=RT [2]=RB [3]=LB
    const float thisLeft   = m_CornersWorldPos[0].x;
    const float thisRight  = m_CornersWorldPos[2].x;
    const float thisTop    = m_CornersWorldPos[0].y;
    const float thisBottom = m_CornersWorldPos[2].y;

    const float otherLeft   = _OtherRect->m_CornersWorldPos[0].x;
    const float otherRight  = _OtherRect->m_CornersWorldPos[2].x;
    const float otherTop    = _OtherRect->m_CornersWorldPos[0].y;
    const float otherBottom = _OtherRect->m_CornersWorldPos[2].y;

    return thisRight > otherLeft  && thisLeft  < otherRight && thisTop   > otherBottom && thisBottom < otherTop;
}

bool CColliderRect::AABBCollision(CColliderCircle* _OtherCircle)
{
    // Rect vs Circle AABB
    const float left   = m_CornersWorldPos[0].x;
    const float right  = m_CornersWorldPos[2].x;
    const float top    = m_CornersWorldPos[0].y;
    const float bottom = m_CornersWorldPos[2].y;

    const Vec3 circleCenter = _OtherCircle->GetWorldPos();

    // 사각형 위에서 원의 중심에 가장 가까운 점 (Closest Point)
    m_CircleClosestPointContacted.x = min(max(left, circleCenter.x), right);
    m_CircleClosestPointContacted.y = min(max(bottom, circleCenter.y), top);
    m_CircleClosestPointContacted.z = 0.f;

    return _OtherCircle->IsCollision(m_CircleClosestPointContacted);
    
    return _OtherCircle->IsCollision(m_CircleClosestPointContacted);
}

bool CColliderRect::AABBCollision(CColliderPoint* _OtherPoint)
{
    // Rect vs Point AABB
    
    const float left   = m_CornersWorldPos[0].x;
    const float right  = m_CornersWorldPos[2].x;
    const float top    = m_CornersWorldPos[0].y;
    const float bottom = m_CornersWorldPos[2].y;

    const Vec3 point = _OtherPoint->GetWorldPos();

    return point.x > left  && point.x < right &&
           point.y > bottom && point.y < top;
}

bool CColliderRect::OBBCollision(CColliderRect* _OtherRect)
{
    Vec3 ProjAxes[4] = { m_CachedRightAxis, m_CachedUpAxis, _OtherRect->m_CachedRightAxis, _OtherRect->m_CachedUpAxis };

    const Vec3 vCenter = _OtherRect->GetWorldPos() - GetWorldPos();

    for (int i = 0; i < 4; ++i)
    {
        const Vec3& vProjAxis = ProjAxes[i];

        float rA = fabsf(vProjAxis.Dot(m_CachedRightAxis)) * m_CachedHalfW + 
                   fabsf(vProjAxis.Dot(m_CachedUpAxis))    * m_CachedHalfH;
                   
        float rB = fabsf(vProjAxis.Dot(_OtherRect->m_CachedRightAxis)) * _OtherRect->m_CachedHalfW + 
                   fabsf(vProjAxis.Dot(_OtherRect->m_CachedUpAxis))    * _OtherRect->m_CachedHalfH;

        if (fabsf(vCenter.Dot(vProjAxis)) > rA + rB)
            return false;
    }

    return true;
}

bool CColliderRect::OBBCollision(CColliderCircle* _OtherCircle)
{
    Vec3 vDiff = _OtherCircle->GetWorldPos() - GetWorldPos();
    vDiff.z = 0.f;

    m_CircleClosestPointContacted = GetWorldPos();
    m_CircleClosestPointContacted.z = 0.f;

    // 각 축에 투영 후 Clamp → Closest Point 계산
    float distRight = vDiff.Dot(m_CachedRightAxis);
    distRight = min(max(distRight, -m_CachedHalfW), m_CachedHalfW);
    m_CircleClosestPointContacted += m_CachedRightAxis * distRight;

    float distUp = vDiff.Dot(m_CachedUpAxis);
    distUp = min(max(distUp, -m_CachedHalfH), m_CachedHalfH);
    m_CircleClosestPointContacted += m_CachedUpAxis * distUp;

    Vec3 vDistVec = _OtherCircle->GetWorldPos() - m_CircleClosestPointContacted;
    vDistVec.z = 0.f;

    const float fRadius = _OtherCircle->GetRadius();
    return vDistVec.LengthSquared() <= fRadius * fRadius;
}

bool CColliderRect::OBBCollision(CColliderPoint* _OtherPoint)
{
    Vec3 vDiff = _OtherPoint->GetWorldPos() - GetWorldPos();
    vDiff.z = 0.f;

    return fabsf(vDiff.Dot(m_CachedRightAxis)) < m_CachedHalfW &&
           fabsf(vDiff.Dot(m_CachedUpAxis))    < m_CachedHalfH;
}

void CColliderRect::SaveToLevelFile(FILE* _File)
{
    CCollider2D::SaveToLevelFile(_File);
    
    fwrite(&m_Scale, sizeof(Vec2), 1, _File);
    fwrite(&m_PivotLocal, sizeof(Vec2), 1, _File);
    
}

void CColliderRect::LoadFromLevelFile(FILE* _File)
{
    CCollider2D::LoadFromLevelFile(_File);
    
    fread(&m_Scale, sizeof(Vec2), 1, _File);
    fread(&m_PivotLocal, sizeof(Vec2), 1, _File);
}
