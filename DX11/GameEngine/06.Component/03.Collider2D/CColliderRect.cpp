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
    CCollider2D::FinalTick();
    
    Matrix matPivotToOrigin = XMMatrixTranslation(-m_PivotLocal.x, -m_PivotLocal.y, 0.f);
    Matrix matScale         = XMMatrixScaling(m_Scale.x, m_Scale.y, 1.f);
    Matrix matPivotBack     = XMMatrixTranslation( m_PivotLocal.x,  m_PivotLocal.y, 0.f);
    Matrix matTranslation   = XMMatrixTranslation(GetOffset().x, GetOffset().y, 0.f);

    // row-vector 기준: T(-Pivot) * S * T(Pivot)
    GetWorldMat() = matPivotToOrigin * matScale * matPivotBack * matTranslation * Transform()->GetWorldMatrix();
    
        
    static const Vec3 kLocalCorners[4] = 
    {
        Vec3(-0.5f,  0.5f, 0.f), // LT
        Vec3( 0.5f,  0.5f, 0.f), // RT
        Vec3( 0.5f, -0.5f, 0.f), // RB
        Vec3(-0.5f, -0.5f, 0.f), // LB
    };
    
    for (int i = 0; i < 4; ++i)
        m_CornersWorldPos[i] = XMVector3TransformCoord(kLocalCorners[i], GetWorldMat());

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
    
    const Vec3 ThisPos          = GetWorldPos();
    const Vec3 ThisWorldScale   = Transform()->GetWorldScale(); 
    const Vec2 ThisSize         = {ThisWorldScale.x * m_Scale.x, ThisWorldScale.y * m_Scale.y};
    
    const Vec3 OtherPos         = _OtherRect->GetWorldPos();
    const Vec3 OtherWorldScale  = _OtherRect->Transform()->GetWorldScale(); 
    const Vec2 OtherSize        = {OtherWorldScale.x * _OtherRect->m_Scale.x, OtherWorldScale.y * _OtherRect->m_Scale.y};
    
    const float x = abs(ThisPos.x - OtherPos.x);
    const float y = abs(ThisPos.y - OtherPos.y);
    
    const float SizeX = (ThisSize.x + OtherSize.x) * 0.5f;
    const float SizeY = (ThisSize.y + OtherSize.y) * 0.5f;

    return x < SizeX && y < SizeY; 
}

bool CColliderRect::AABBCollision(CColliderCircle* _OtherCircle)
{
    // Rect vs Circle AABB

    const Vec3 CircleMid = _OtherCircle->GetWorldPos();

    static Ptr<AMesh> pRectMesh{};
    if (!pRectMesh) pRectMesh = FIND_ASSET(AMesh, L"RectMesh");
    
    const Vtx* pVtx = pRectMesh->GetVtxSysMem();
    
    const Vec3 ThisLeftTop      = XMVector3TransformCoord(pVtx[0].vPos, this->GetWorldMat());
    const Vec3 ThisRightBottom  = XMVector3TransformCoord(pVtx[2].vPos, this->GetWorldMat());
    
    const float Left            = ThisLeftTop.x;
    const float Right           = ThisRightBottom.x;
    const float Top             = ThisLeftTop.y;
    const float Bottom          = ThisRightBottom.y;
    
    m_CircleClosestPointContacted.x = min(max(Left, CircleMid.x), Right);
    m_CircleClosestPointContacted.y = min(max(Bottom, CircleMid.y), Top);
    
    return _OtherCircle->IsCollision(m_CircleClosestPointContacted);
}

bool CColliderRect::AABBCollision(CColliderPoint* _OtherPoint)
{
    // Rect vs Point AABB
    
    static Ptr<AMesh> pRectMesh{};
    if (!pRectMesh) pRectMesh = FIND_ASSET(AMesh, L"RectMesh");
    
    const Vtx* pVtx = pRectMesh->GetVtxSysMem();
    
    const Vec3 ThisPos          = GetWorldPos();
    const Vec3 ThisWorldScale   = Transform()->GetWorldScale(); 
    const Vec2 ThisSize         = {ThisWorldScale.x * m_Scale.x, ThisWorldScale.y * m_Scale.y};
    
    const Vec3 OtherPointPos = _OtherPoint->GetWorldPos();
    
    float x = abs(OtherPointPos.x - ThisPos.x);
    float y = abs(OtherPointPos.y - ThisPos.y);
    
    return (x < ThisSize.x * 0.5f) && (y < ThisSize.y * 0.5f);   
}

bool CColliderRect::OBBCollision(CColliderRect* _OtherRect)
{
    // Rect vs Rect OBB
    static Ptr<AMesh> pRectMesh{};
    if (!pRectMesh) pRectMesh = FIND_ASSET(AMesh, L"RectMesh");

    const Vtx* pVtx             = pRectMesh->GetVtxSysMem();
    const Matrix& matWorldLeft  = this->GetWorldMat();
    const Matrix& matWorldRight =_OtherRect->GetWorldMat();

    // 월드 공간상에서 충돌을 검사하기 위해서, RectMesh 모델을 각 충돌체의 월드행렬을 곱해서 정점을 충돌체 꼭지점에 배치시킨다.
    // 각 꼭지점끼리 빼서 두 충돌체의 표면 방향벡터를 각 충돌체로부터 2개씩 구한다.
    
    Vec3 Axes[4]{};
    
    // Left collider 축 초기화
    Axes[0] = XMVector3TransformCoord(pVtx[1].vPos, matWorldLeft) - XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);
    Axes[1] = XMVector3TransformCoord(pVtx[3].vPos, matWorldLeft) - XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);

    // Right Collider 축 초기화
    Axes[2] = XMVector3TransformCoord(pVtx[1].vPos, matWorldRight) - XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);
    Axes[3] = XMVector3TransformCoord(pVtx[3].vPos, matWorldRight) - XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);

    Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matWorldRight) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matWorldLeft);

    
    for (int i = 0; i < 4; ++i)
    {
        // 4개의 축 중에서, 하나를 투영 목적지로 정함
        // 원본값을 훼손하면 나중에 투영할 때 문제가 생기기 때문에, 정규화한 벡터를 따로 지역변수로 둠
        Vec3 vProjAxis = Axes[i];
        vProjAxis.Normalize();
        
        // 투영축으로 4개의 벡터를 투영시켜서 얻은 면적의 절반 길이를 구함
        float Dot{};
        
        for (const Vec3& Ax : Axes)
            Dot += fabs(vProjAxis.Dot(Ax));
        
        Dot *= 0.5f;
        
        // 두 충돌체 중심을 잇는 벡터도 투영
        float fCenter = fabs(vCenter.Dot(vProjAxis));
        
        if (fCenter > Dot) return false;
    }
    
    return true;
}

bool CColliderRect::OBBCollision(CColliderCircle* _OtherCircle)
{
    Vec3 vAxes[2]{};
    vAxes[0] = GetWorldMat().Right();
    vAxes[1] = GetWorldMat().Up();
    vAxes[0].Normalize();
    vAxes[1].Normalize();

    const Vec2 vHalfSize = Transform()->GetWorldScale() * m_Scale * 0.5f;
    
    Vec3 vDiff = _OtherCircle->GetWorldPos() - this->GetWorldPos();
    vDiff.z = 0.f;
    m_CircleClosestPointContacted = this->GetWorldPos();
    m_CircleClosestPointContacted.z = 0.f;

    for (int i = 0; i < 2; ++i)
    {
        // 원의 중심을 각 축에 투영
        float fDist = vDiff.Dot(vAxes[i]);

        // 투영된 거리를 사각형 범위 내로 제한(Clamp)
        if (fDist > vHalfSize[i])       fDist = vHalfSize[i];
        else if (fDist < -vHalfSize[i]) fDist = -vHalfSize[i];

        // 제한된 거리만큼 축 방향으로 이동하여 Closest Point 업데이트
        m_CircleClosestPointContacted += vAxes[i] * fDist;
    }

    Vec3 vDistVec = _OtherCircle->GetWorldPos() - m_CircleClosestPointContacted;
    vDistVec.z = 0.f;
    
    const float fDistSq = vDistVec.LengthSquared(); 
    const float fRadius = _OtherCircle->GetRadius();
    
    return fDistSq <= (fRadius * fRadius);
}

bool CColliderRect::OBBCollision(CColliderPoint* _OtherPoint)
{
    // Rect vs Point OBB
    
    const Matrix InvWorld = XMMatrixInverse(nullptr, GetWorldMat()); // 여기서 Scale까지 원상복구 처리되는 Matrix가 생성이 되어버림

    Vec3 InvPoint = XMVector3TransformCoord(_OtherPoint->GetWorldPos(), InvWorld);
    InvPoint.x *= m_Scale.x * Transform()->GetWorldScale().x; // 총 Scale 곱만 원상복구
    InvPoint.y *= m_Scale.y * Transform()->GetWorldScale().y;
    
    
    const Vec3 ThisWorldScale = Transform()->GetWorldScale(); 
    const Vec2 RectWorldScale = {ThisWorldScale.x * m_Scale.x, ThisWorldScale.y * m_Scale.y};
    
    return abs(InvPoint.x) < RectWorldScale.x * 0.5f && abs(InvPoint.y) < RectWorldScale.y * 0.5f; 
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
