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

void CollisionMgr::RayCastFindBestHit
(
    UINT                _TargetLayerMask,
    const Ray2D&        _Ray,
    bool&               _bOutFoundHit,
    RayCastHit&         _OutBestHit,
    const CCollider2D*  _Ignore
)
{
    const vector<Ptr<GameObject>>& vecParentObjects = LevelMgr::GetInst()->GetCurLevel()->GetLayer(_TargetLayerMask)->GetParentObjects();
    
    for (const Ptr<GameObject>& Parent : vecParentObjects)
    {
        queue<Ptr<GameObject>> q{};
        q.push(Parent);
        
        while (!q.empty())
        {
            Ptr<GameObject> Object = q.front(); q.pop();
            
            for (const Ptr<GameObject>& child : Object->GetChildren())
                q.push(child);
            
            if 
            (
                !Object->GetActive() ||
                !Object->GetCollider2D() ||
                !Object->GetCollider2D()->GetActive() ||
                Object->IsObjectDestroyed()
            ) continue;

            Ptr<CCollider2D> pCollider = Object->GetCollider2D();
            if (pCollider.Get() == _Ignore)                                        continue;
            if (pCollider->GetComponentType() == COMPONENT_TYPE::COLLIDER2D_POINT) continue;

            bool bHit{};
            float fDistance{};
            Vec2 vPoint{}, vNormal{};

            if (pCollider->GetComponentType() == COMPONENT_TYPE::COLLIDER2D_RECT)
            {
                CColliderRect* pRect = static_cast<CColliderRect*>(pCollider.Get());
                bHit = RayVsRectOBB(_Ray, pRect->GetWorldMat(), fDistance, vPoint, vNormal);
            }
            else if (pCollider->GetComponentType() == COMPONENT_TYPE::COLLIDER2D_CIRCLE)
            {
                CColliderCircle* pCircle = static_cast<CColliderCircle*>(pCollider.Get());
                bHit = RayVsCircle(_Ray, pCircle, fDistance, vPoint, vNormal);
            }

            if (!bHit) continue;
            if (fDistance < 0.f || fDistance > _Ray.MaxDistance) continue;

            if (!_bOutFoundHit || fDistance < _OutBestHit.Distance)
            {
                _bOutFoundHit = true;
                _OutBestHit.Collider    = pCollider;
                _OutBestHit.Point       = vPoint;
                _OutBestHit.Normal      = vNormal;
                _OutBestHit.Distance    = fDistance;
            }
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
    const Matrix&   _RectWorldMat,
    float&          _OutT,
    Vec2&           _OutPoint,
    Vec2&           _OutNormal
)
{
    constexpr float fMinX = -0.5f;
    constexpr float fMaxX = 0.5f;
    constexpr float fMinY = -0.5f;
    constexpr float fMaxY = 0.5f;
    
    
    // Z scale이 0인 경우 XMMatrixInverse 실패 가능 -> Vec2 역변환 사용
    const Vec2 vCenter  = ToVec2(_RectWorldMat.Translation());
    const Vec2 axisX    = Vec2(_RectWorldMat.Right().x, _RectWorldMat.Right().y);
    const Vec2 axisY    = Vec2(_RectWorldMat.Up().x,    _RectWorldMat.Up().y);

    const float fDet = axisX.x * axisY.y - axisX.y * axisY.x;
    
    if (fabsf(fDet) < RAY_EPSILON)
        return false;

    const float invDet = 1.f / fDet;

    // inv([axisX axisY]) (column basis)
    const float inv00 = axisY.y * invDet;
    const float inv01 = -axisY.x * invDet;
    const float inv10 = -axisX.y * invDet;
    const float inv11 = axisX.x * invDet;

    const Vec2 relOrigin = Vec2(_Ray.Origin.x - vCenter.x, _Ray.Origin.y - vCenter.y);
    const Vec2 rayDir2D = Vec2(_Ray.Direction.x, _Ray.Direction.y);

    Vec3 vLocalOrigin{};
    vLocalOrigin.x = inv00 * relOrigin.x + inv01 * relOrigin.y;
    vLocalOrigin.y = inv10 * relOrigin.x + inv11 * relOrigin.y;

    Vec3 vLocalDir{};
    vLocalDir.x = inv00 * rayDir2D.x + inv01 * rayDir2D.y;
    vLocalDir.y = inv10 * rayDir2D.x + inv11 * rayDir2D.y;

    float tMin = 0.f;
    float tMax = _Ray.MaxDistance;

    for (int axis = 0; axis < 2; ++axis)
    {
        const float origin = (axis == 0) ? vLocalOrigin.x : vLocalOrigin.y;
        const float dir    = (axis == 0) ? vLocalDir.x    : vLocalDir.y;
        const float minB   = (axis == 0) ? fMinX          : fMinY;
        const float maxB   = (axis == 0) ? fMaxX          : fMaxY;

        if (fabsf(dir) < RAY_EPSILON)
        {
            if (origin < minB || origin > maxB) return false;
            continue;
        }

        float t1 = (minB - origin) / dir;
        float t2 = (maxB - origin) / dir;

        if (t1 > t2)
        {
            const float temp = t1;
            t1 = t2;
            t2 = temp;
        }

        tMin = max(t1, tMin);
        tMax = min(t2, tMax);

        if (tMin > tMax) return false;
    }

    float tHit = tMin;
    if (tHit < 0.f)
    {
        if (tMax < 0.f) return false;
            

        tHit = tMax;
    }

    if (tHit > _Ray.MaxDistance)
        return false;

    _OutT = tHit;
    _OutPoint = _Ray.Origin + _Ray.Direction * tHit;

    // 로컬 히트점을 이용해 면 노멀 결정
    const Vec3 vLocalHit = vLocalOrigin + vLocalDir * tHit;
    const float dxMin = fabsf(vLocalHit.x - fMinX);
    const float dxMax = fabsf(vLocalHit.x - fMaxX);
    const float dyMin = fabsf(vLocalHit.y - fMinY);
    const float dyMax = fabsf(vLocalHit.y - fMaxY);

    Vec3 vLocalNormal = Vec3(1.f, 0.f, 0.f);
    float fBest = dxMax;

    if (dxMin < fBest)
    {
        fBest = dxMin;
        vLocalNormal = Vec3(-1.f, 0.f, 0.f);
    }

    if (dyMax < fBest)
    {
        fBest = dyMax;
        vLocalNormal = Vec3(0.f, 1.f, 0.f);
    }

    if (dyMin < fBest)
    {
        vLocalNormal = Vec3(0.f, -1.f, 0.f);
    }
    
    _OutNormal = _RectWorldMat.Right() * vLocalNormal.x + _RectWorldMat.Up() * vLocalNormal.y;
    if (_OutNormal.LengthSquared() > RAY_EPSILON * RAY_EPSILON)
        _OutNormal.Normalize();
    else
        _OutNormal = -_Ray.Direction;

    return true;
}