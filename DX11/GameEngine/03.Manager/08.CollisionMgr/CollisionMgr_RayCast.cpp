#include "pch.h"
#include "CollisionMgr.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderCircle.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderRect.h"


bool CollisionMgr::RayCast
(
    const Ray2D&        _Ray,
    UINT                _TargetLayerMask,
    RayCastHit*         _OutHit,
    const CCollider2D*  _Ignore
)
{

    Ray2D Ray = _Ray;

    if (_OutHit) *_OutHit = {}; // OutHit 초기화
    
    if (Ray.MaxDistance <= 0.f) Ray.MaxDistance = FLT_MAX;

    if (Ray.Direction.LengthSquared() <= RAY_EPSILON * RAY_EPSILON)
        return false;

    Ray.Direction.Normalize();

    bool bFoundHit{};
    RayCastHit BestHit{}; BestHit.Distance = FLT_MAX;

    RayCastFindBestHit(_TargetLayerMask, Ray, bFoundHit, BestHit, _Ignore);

    if (bFoundHit && _OutHit) *_OutHit = BestHit;

    return bFoundHit;
}

bool CollisionMgr::RayCast
(
    const Ray2D&        _Ray,
    const vector<UINT>& _vecTargetLayerMask,
    RayCastHit*         _OutHit,
    const CCollider2D*  _Ignore
)
{
    Ray2D Ray = _Ray;
    
    if (_OutHit) *_OutHit = {}; // OutHit 초기화

    if (Ray.MaxDistance <= 0.f) Ray.MaxDistance = FLT_MAX;

    if (Ray.Direction.LengthSquared() <= RAY_EPSILON * RAY_EPSILON)
        return false;

    Ray.Direction.Normalize();

    bool bFoundHit{};
    RayCastHit BestHit{}; BestHit.Distance = FLT_MAX;

    for (UINT TargetLayerMask : _vecTargetLayerMask)
        RayCastFindBestHit(TargetLayerMask, Ray, bFoundHit, BestHit, _Ignore);
    
    if (bFoundHit && _OutHit) *_OutHit = BestHit;
    
    return bFoundHit;
}

bool CollisionMgr::RayCast
(
    const Ray2D&        _Ray,
    const set<UINT>&    _setTargetLayerMask,
    RayCastHit*         _OutHit,
    const CCollider2D*  _Ignore
)
{
    Ray2D Ray = _Ray;
    
    if (_OutHit) *_OutHit = {}; // OutHit 초기화

    if (Ray.MaxDistance <= 0.f) Ray.MaxDistance = FLT_MAX;

    if (Ray.Direction.LengthSquared() <= RAY_EPSILON * RAY_EPSILON)
        return false;

    Ray.Direction.Normalize();

    bool bFoundHit{};
    RayCastHit BestHit{}; BestHit.Distance = FLT_MAX;

    for (UINT TargetLayerMask : _setTargetLayerMask)
        RayCastFindBestHit(TargetLayerMask, Ray, bFoundHit, BestHit, _Ignore);
    
    if (bFoundHit && _OutHit) *_OutHit = BestHit;
    
    return bFoundHit;
}

void CollisionMgr::RayCastFindBestHit
(
    UINT                _TargetLayerMask,
    const Ray2D&        _Ray,
    bool&               _bOutFoundHit,
    RayCastHit&         _OutBestHit,
    const CCollider2D*  _Ignore
)
{
    const CellBuckets& buckets = m_arrLayerBuckets[_TargetLayerMask];
    if (buckets.empty()) return;

    // DDA variables
    float stepX = (_Ray.Direction.x > 0) ? 1.0f : -1.0f;
    float stepY = (_Ray.Direction.y > 0) ? 1.0f : -1.0f;

    int cellX = WorldToCellCoord(_Ray.Origin.x);
    int cellY = WorldToCellCoord(_Ray.Origin.y);

    // tDelta: 한 격자 크기만큼 이동할 때 t가 변하는 양
    float tDeltaX = (fabsf(_Ray.Direction.x) > RAY_EPSILON) ? fabsf(m_GridCellSize / _Ray.Direction.x) : FLT_MAX;
    float tDeltaY = (fabsf(_Ray.Direction.y) > RAY_EPSILON) ? fabsf(m_GridCellSize / _Ray.Direction.y) : FLT_MAX;

    float tMaxX, tMaxY;
    // 현재 셀에서의 tMax 계산
    if (_Ray.Direction.x > 0)
        tMaxX = (floorf(_Ray.Origin.x / m_GridCellSize + 1.f) * m_GridCellSize - _Ray.Origin.x) / _Ray.Direction.x;
    else if (_Ray.Direction.x < 0)
        tMaxX = (floorf(_Ray.Origin.x / m_GridCellSize) * m_GridCellSize - _Ray.Origin.x) / _Ray.Direction.x;
    else
        tMaxX = FLT_MAX;

    if (_Ray.Direction.y > 0)
        tMaxY = (floorf(_Ray.Origin.y / m_GridCellSize + 1.f) * m_GridCellSize - _Ray.Origin.y) / _Ray.Direction.y;
    else if (_Ray.Direction.y < 0)
        tMaxY = (floorf(_Ray.Origin.y / m_GridCellSize) * m_GridCellSize - _Ray.Origin.y) / _Ray.Direction.y;
    else
        tMaxY = FLT_MAX;

    // 중복 검사 방환용 (동일 프레임 내에서 같은 콜라이더를 여러 격자에서 만날 수 있음)
    static vector<UINT> vecCheckedIDs;
    vecCheckedIDs.clear();

    float currentT = 0.0f;
    const float maxT = _Ray.MaxDistance;

    // 루프 제한 (무한 루프 방지용 안전장치)
    int maxSteps = 2000; 

    while (currentT <= maxT && maxSteps-- > 0)
    {
        ULONGLONG key = MakeCellKey(cellX, cellY);
        auto it = buckets.find(key);
        if (it != buckets.end())
        {
            for (UINT id : it->second)
            {
                // 이미 검사했는지 확인
                bool bAlreadyChecked = false;
                for (size_t i = 0; i < vecCheckedIDs.size(); ++i) {
                    if (vecCheckedIDs[i] == id) { bAlreadyChecked = true; break; }
                }
                if (bAlreadyChecked) continue;
                vecCheckedIDs.push_back(id);

                const auto iterCol = m_mapColliderByID.find(id);
                if (iterCol == m_mapColliderByID.end()) continue;
                
                Ptr<CCollider2D> pCollider = iterCol->second;
                if (!pCollider || !pCollider->GetActive() || pCollider.Get() == _Ignore) continue;

                bool bHit = false;
                float fDist = 0.f;
                Vec2 vPoint{}, vNormal{};

                if (pCollider->GetComponentType() == COMPONENT_TYPE::COLLIDER2D_RECT)
                {
                    CColliderRect* pRect = static_cast<CColliderRect*>(pCollider.Get());
                    bHit = RayVsRectOBB(_Ray, pRect, fDist, vPoint, vNormal);
                }
                else if (pCollider->GetComponentType() == COMPONENT_TYPE::COLLIDER2D_CIRCLE)
                {
                    CColliderCircle* pCircle = static_cast<CColliderCircle*>(pCollider.Get());
                    bHit = RayVsCircle(_Ray, pCircle, fDist, vPoint, vNormal);
                }

                if (bHit && fDist >= 0.f && fDist <= maxT)
                {
                    if (!_bOutFoundHit || fDist < _OutBestHit.Distance)
                    {
                        _bOutFoundHit = true;
                        _OutBestHit.Collider = pCollider;
                        _OutBestHit.Point = vPoint;
                        _OutBestHit.Normal = vNormal;
                        _OutBestHit.Distance = fDist;
                    }
                }
            }
        }

        // 이번 격자(와 이전 격자들)에서 충돌이 발견되었고, 
        // 그 충돌 지점이 다음 격자 경계(tMax)보다 가까우면 더 이상 검사할 필요 없음 (순서 보장)
        if (_bOutFoundHit && _OutBestHit.Distance <= min(tMaxX, tMaxY))
            break;

        // 다음 격자로 이동
        if (tMaxX < tMaxY)
        {
            currentT = tMaxX;
            tMaxX += tDeltaX;
            cellX += (int)stepX;
        }
        else
        {
            currentT = tMaxY;
            tMaxY += tDeltaY;
            cellY += (int)stepY;
        }
    }
}

bool CollisionMgr::RayVsCircle(const Ray2D& _Ray, CColliderCircle* _Circle, float& _OutT, Vec2& _OutPoint, Vec2& _OutNormal)
{
    Vec2 vOrigin = _Ray.Origin;
    Vec2 vCenter = ToVec2(_Circle->GetWorldPos());

    const float fRadius = _Circle->GetRadius();
    const Vec2 vOC = vOrigin - vCenter;

    // (oc + dir * t)^2 = r^2
    const float b = vOC.Dot(_Ray.Direction);
    const float c = vOC.LengthSquared() - fRadius * fRadius;
    const float discriminant = b * b - c;

    if (discriminant < 0.f) return false;

    const float sqrtD = sqrtf(discriminant);
    const float t0 = -b - sqrtD;
    const float t1 = -b + sqrtD;

    float tHit = -1.f;
    if (t0 >= 0.f)      tHit = t0;
    else if (t1 >= 0.f) tHit = t1;
    else                return false;

    if (tHit > _Ray.MaxDistance) return false;

    _OutT = tHit;
    _OutPoint = vOrigin + _Ray.Direction * tHit;

    _OutNormal = _OutPoint - vCenter;
    if (_OutNormal.LengthSquared() > RAY_EPSILON * RAY_EPSILON)
        _OutNormal.Normalize();
    else
        _OutNormal = -_Ray.Direction;

    return true;
}

bool CollisionMgr::RayVsRectOBB
(
    const Ray2D&    _Ray,
    CColliderRect*  _Rect,
    float&          _OutT,
    Vec2&           _OutPoint,
    Vec2&           _OutNormal
)
{
    // 로컬 좌표계로 변환 (OBB의 중심과 축 활용)
    const Vec2 vCenter = ToVec2(_Rect->GetWorldPos());
    const Vec2 axisX = ToVec2(_Rect->m_CachedRightAxis);
    const Vec2 axisY = ToVec2(_Rect->m_CachedUpAxis);
    const float fHalfW = _Rect->m_CachedHalfW;
    const float fHalfH = _Rect->m_CachedHalfH;

    const Vec2 relOrigin = _Ray.Origin - vCenter;

    // 로컬 원점 및 방향 계산 (역행렬 대신 내적 사용)
    Vec2 vLocalOrigin = Vec2(relOrigin.Dot(axisX), relOrigin.Dot(axisY));
    Vec2 vLocalDir = Vec2(_Ray.Direction.Dot(axisX), _Ray.Direction.Dot(axisY));

    float tMin = 0.f;
    float tMax = _Ray.MaxDistance;

    // AABB vs Ray (Local Space)
    float minB[2] = { -fHalfW, -fHalfH };
    float maxB[2] = {  fHalfW,  fHalfH };
    float origin[2] = { vLocalOrigin.x, vLocalOrigin.y };
    float dir[2] = { vLocalDir.x, vLocalDir.y };

    for (int i = 0; i < 2; ++i)
    {
        if (fabsf(dir[i]) < RAY_EPSILON)
        {
            if (origin[i] < minB[i] || origin[i] > maxB[i]) return false;
        }
        else
        {
            float t1 = (minB[i] - origin[i]) / dir[i];
            float t2 = (maxB[i] - origin[i]) / dir[i];

            if (t1 > t2) { float temp = t1; t1 = t2; t2 = temp; }
            tMin = max(tMin, t1);
            tMax = min(tMax, t2);

            if (tMin > tMax) return false;
        }
    }

    float tHit = tMin;
    if (tHit < 0.f)
    {
        if (tMax < 0.f) return false;
        tHit = tMax;
    }

    if (tHit > _Ray.MaxDistance) return false;

    _OutT = tHit;
    _OutPoint = _Ray.Origin + _Ray.Direction * tHit;

    // Normal 결정
    Vec2 vLocalHit = vLocalOrigin + vLocalDir * tHit;
    float dxMin = fabsf(vLocalHit.x - minB[0]);
    float dxMax = fabsf(vLocalHit.x - maxB[0]);
    float dyMin = fabsf(vLocalHit.y - minB[1]);
    float dyMax = fabsf(vLocalHit.y - maxB[1]);

    float fBest = dxMax;
    _OutNormal = axisX;

    if (dxMin < fBest)
    {
        fBest = dxMin;
        _OutNormal = -axisX;
    }
    if (dyMax < fBest)
    {
        fBest = dyMax;
        _OutNormal = axisY;
    }
    if (dyMin < fBest)
    {
        _OutNormal = -axisY;
    }

    return true;
}