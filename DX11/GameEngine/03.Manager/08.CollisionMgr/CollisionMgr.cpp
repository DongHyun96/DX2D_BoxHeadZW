#include "pch.h"
#include "CollisionMgr.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderCircle.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderPoint.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderRect.h"


CollisionMgr::CollisionMgr()
{
    
}

CollisionMgr::~CollisionMgr()
{
}

void CollisionMgr::Progress(const Ptr<ALevel>& _Level)
{
    ++m_FrameCounter;
    BuildLayerColliderCache(_Level);
    
    UINT* pMatrix = _Level->GetCollisionMatrix();
    
    for (UINT Row = 0; Row < MAX_LAYER; ++Row)
    {
        for (UINT Col = Row; Col < MAX_LAYER; ++Col)
        {
            if (false == (pMatrix[Row] & (1 << Col))) continue;
            CollisionBtwLayer(Row, Col);
        }
    }

    ProcessStaleCollisionPairs();
    PruneCollisionPairCache();
}

void CollisionMgr::BuildLayerColliderCache(const Ptr<ALevel>& _Level)
{
    m_mapColliderByID.clear();
    
    for (UINT layerIdx = 0; layerIdx < MAX_LAYER; ++layerIdx)
    {
        auto& vecLayerColliders = m_arrLayerColliders[layerIdx];
        auto& layerBuckets = m_arrLayerBuckets[layerIdx];
        vecLayerColliders.clear();
        layerBuckets.clear();
        
        const vector<Ptr<GameObject>>& vecAllObjects = _Level->GetLayer(layerIdx)->GetAllObjects();
        vecLayerColliders.reserve(vecAllObjects.size());
        
        for (const Ptr<GameObject>& object : vecAllObjects)
        {
            Ptr<CCollider2D> pCollider = object->GetCollider2D();
            if (!pCollider) continue;
            
            vecLayerColliders.push_back(pCollider);
            m_mapColliderByID[pCollider->GetEntityInstID()] = pCollider;

            if (!object->GetActive() || object->IsObjectDestroyed() || !pCollider->GetActive()) continue;

            // Optimization: If non-movable and cache is valid, reuse cell keys
            if (!pCollider->GetMovable() && pCollider->m_bCacheValid && !pCollider->m_vecCachedCellKeys.empty())
            {
                for (ULONGLONG cellKey : pCollider->m_vecCachedCellKeys)
                {
                    layerBuckets[cellKey].push_back(pCollider->GetEntityInstID());
                }
                continue;
            }

            Vec2 vMin{}, vMax{};
            if (!TryGetColliderAABB(pCollider, vMin, vMax))
                continue;

            const int minCellX = WorldToCellCoord(vMin.x);
            const int maxCellX = WorldToCellCoord(vMax.x);
            const int minCellY = WorldToCellCoord(vMin.y);
            const int maxCellY = WorldToCellCoord(vMax.y);

            if (!pCollider->GetMovable())
                pCollider->m_vecCachedCellKeys.clear();

            for (int y = minCellY; y <= maxCellY; ++y)
            {
                for (int x = minCellX; x <= maxCellX; ++x)
                {
                    ULONGLONG cellKey = MakeCellKey(x, y);
                    layerBuckets[cellKey].push_back(pCollider->GetEntityInstID());
                    
                    if (!pCollider->GetMovable())
                        pCollider->m_vecCachedCellKeys.push_back(cellKey);
                }
            }

            if (!pCollider->GetMovable())
                pCollider->m_bCacheValid = true;
        }
    }
}

int CollisionMgr::WorldToCellCoord(float _Coord) const
{
    return static_cast<int>(floorf(_Coord / m_GridCellSize));
}

ULONGLONG CollisionMgr::MakeCellKey(int _CellX, int _CellY) const
{
    return (static_cast<ULONGLONG>(static_cast<UINT>(_CellX)) << 32) | static_cast<UINT>(_CellY);
}

ULONGLONG CollisionMgr::MakePairKey(UINT _A, UINT _B) const
{
    const UINT leftID = (_A < _B) ? _A : _B;
    const UINT rightID = (_A < _B) ? _B : _A;

    COL_ID pairID{};
    pairID.LeftID = leftID;
    pairID.RightID = rightID;
    return pairID.ID;
}

bool CollisionMgr::TryGetColliderAABB(const Ptr<CCollider2D>& _Collider, Vec2& _OutMin, Vec2& _OutMax)
{
    if (!_Collider) return false;

    if (!_Collider->GetMovable() && _Collider->m_bCacheValid)
    {
        _OutMin = _Collider->m_vCachedMin;
        _OutMax = _Collider->m_vCachedMax;
        return true;
    }

    bool bSuccess = false;
    switch (_Collider->GetComponentType())
    {
    case COMPONENT_TYPE::COLLIDER2D_RECT:
    {
        CColliderRect* pRect = static_cast<CColliderRect*>(_Collider.Get());
        Vec3 corner = pRect->GetCornerWorldPos(0);
        float minX = corner.x;
        float maxX = corner.x;
        float minY = corner.y;
        float maxY = corner.y;

        for (UINT i = 1; i < 4; ++i)
        {
            corner = pRect->GetCornerWorldPos(i);
            minX = min(minX, corner.x);
            maxX = max(maxX, corner.x);
            minY = min(minY, corner.y);
            maxY = max(maxY, corner.y);
        }

        _OutMin = Vec2(minX, minY);
        _OutMax = Vec2(maxX, maxY);
        bSuccess = true;
        break;
    }
    case COMPONENT_TYPE::COLLIDER2D_CIRCLE:
    {
        CColliderCircle* pCircle = static_cast<CColliderCircle*>(_Collider.Get());
        const Vec3 vCenter = pCircle->GetWorldPos();
        const float fRadius = pCircle->GetRadius();
        
        _OutMin = Vec2(vCenter.x - fRadius, vCenter.y - fRadius);
        _OutMax = Vec2(vCenter.x + fRadius, vCenter.y + fRadius);
        bSuccess = true;
        break;
    }
    case COMPONENT_TYPE::COLLIDER2D_POINT:
    {
        CColliderPoint* pPoint = static_cast<CColliderPoint*>(_Collider.Get());
        const Vec3 vCenter = pPoint->GetWorldPos();
        _OutMin = Vec2(vCenter.x, vCenter.y);
        _OutMax = _OutMin;
        bSuccess = true;
        break;
    }
    default:
        return false;
    }

    if (bSuccess)
    {
        _Collider->m_vCachedMin = _OutMin;
        _Collider->m_vCachedMax = _OutMax;
        // m_bCacheValid는 BuildLayerColliderCache에서 Cell Key까지 채운 후 true로 설정하거나, 여기서 일단 true로 할 수 있음.
        // 하지만 Cell Key 캐시와 동기화하기 위해 BuildLayerColliderCache에서 관리하는 것이 나을 듯.
    }

    return bSuccess;
}

void CollisionMgr::CollisionBtwLayer(UINT _LeftLayerIdx, UINT _RightLayerIdx)
{
    if (_LeftLayerIdx == _RightLayerIdx)
    {
        CollisionBtwSameLayer(_LeftLayerIdx);
        return;
    }

    const CellBuckets& leftBuckets  = m_arrLayerBuckets[_LeftLayerIdx];
    const CellBuckets& rightBuckets = m_arrLayerBuckets[_RightLayerIdx];

    if (leftBuckets.empty() || rightBuckets.empty())
        return;

    const CellBuckets* pIterBuckets  = &leftBuckets;
    const CellBuckets* pOtherBuckets = &rightBuckets;
    bool bIterIsLeft                 = true;
    
    if (leftBuckets.size() > rightBuckets.size())
    {
        pIterBuckets  = &rightBuckets;
        pOtherBuckets = &leftBuckets;
        bIterIsLeft   = false;
    }

    unordered_set<ULONGLONG> setCandidatePairs{};

    for (const auto& [cellKey, vecA] : *pIterBuckets)
    {
        const auto iterOther = pOtherBuckets->find(cellKey);
        if (iterOther == pOtherBuckets->end()) continue;

        const vector<UINT>& vecB = iterOther->second;

        for (UINT idA : vecA)
        {
            for (UINT idB : vecB)
            {
                const UINT leftID       = bIterIsLeft ? idA : idB;
                const UINT rightID      = bIterIsLeft ? idB : idA;
                const ULONGLONG pairKey = MakePairKey(leftID, rightID);

                if (!setCandidatePairs.insert(pairKey).second)
                    continue;

                const auto iterLeftCol  = m_mapColliderByID.find(leftID);
                const auto iterRightCol = m_mapColliderByID.find(rightID);
                
                if (iterLeftCol == m_mapColliderByID.end() || iterRightCol == m_mapColliderByID.end()) continue;

                CheckCollisionAndNotify(iterLeftCol->second, iterRightCol->second);
            }
        }
    }
}

void CollisionMgr::CollisionBtwSameLayer(UINT _LayerIdx)
{
    const CellBuckets& buckets = m_arrLayerBuckets[_LayerIdx];
    if (buckets.empty()) return;

    unordered_set<ULONGLONG> setCandidatePairs{};
    
    for (const auto& [cellKey, vecIDs] : buckets)
    {
        if (vecIDs.size() < 2) continue;

        for (int i = 0; i < static_cast<int>(vecIDs.size()); ++i)
        {
            for (int j = i + 1; j < static_cast<int>(vecIDs.size()); ++j)
            {
                const UINT leftID = vecIDs[i];
                const UINT rightID = vecIDs[j];
                const ULONGLONG pairKey = MakePairKey(leftID, rightID);

                if (!setCandidatePairs.insert(pairKey).second)
                    continue;

                const auto iterLeftCol = m_mapColliderByID.find(leftID);
                const auto iterRightCol = m_mapColliderByID.find(rightID);
                if (iterLeftCol == m_mapColliderByID.end() || iterRightCol == m_mapColliderByID.end())
                    continue;

                CheckCollisionAndNotify(iterLeftCol->second, iterRightCol->second);
            }
        }
    }
}

void CollisionMgr::CheckCollisionAndNotify(const Ptr<CCollider2D>& _LeftCol, const Ptr<CCollider2D>& _RightCol)
{
    if (!_LeftCol || !_RightCol) return;

    const UINT leftID            = _LeftCol->GetEntityInstID();
    const UINT rightID           = _RightCol->GetEntityInstID();
    const UINT normalizedLeftID  = (leftID < rightID) ? leftID : rightID;
    const UINT normalizedRightID = (leftID < rightID) ? rightID : leftID;
    const ULONGLONG pairKey      = MakePairKey(leftID, rightID);

    auto [iterState, bInserted] = m_mapColState.try_emplace(pairKey, CollisionPairState{});
    CollisionPairState& pairState = iterState->second;
    if (bInserted)
    {
        pairState.LeftID = normalizedLeftID;
        pairState.RightID = normalizedRightID;
    }
    
    pairState.LastSeenFrame = m_FrameCounter;

    // Optimization: If both are non-movable and already have cached state, skip actual collision testing
    bool bColliding = false;
    if (!bInserted && !_LeftCol->GetMovable() && !_RightCol->GetMovable())
    {
        bColliding = pairState.IsColliding;
    }
    else
    {
        bColliding = _LeftCol->IsCollision(_RightCol);
    }
    
    if (bColliding)
    {
        if (pairState.IsColliding)
        {
            _LeftCol->Overlap(_RightCol);
            _RightCol->Overlap(_LeftCol);
        }
        else
        {
            _LeftCol->BeginOverlap(_RightCol);
            _RightCol->BeginOverlap(_LeftCol);
        }

        pairState.IsColliding = true;
    }
    else
    {
        if (pairState.IsColliding)
        {
            _LeftCol->EndOverlap(_RightCol);
            _RightCol->EndOverlap(_LeftCol);
        }

        pairState.IsColliding = false;
    }
}

void CollisionMgr::ProcessStaleCollisionPairs()
{
    for (auto& [pairKey, pairState] : m_mapColState)
    {
        if (pairState.LastSeenFrame == m_FrameCounter) continue;
        if (!pairState.IsColliding) continue;

        const auto iterLeftCol = m_mapColliderByID.find(pairState.LeftID);
        const auto iterRightCol = m_mapColliderByID.find(pairState.RightID);

        if (iterLeftCol != m_mapColliderByID.end() && iterRightCol != m_mapColliderByID.end())
        {
            iterLeftCol->second->EndOverlap(iterRightCol->second);
            iterRightCol->second->EndOverlap(iterLeftCol->second);
        }

        pairState.IsColliding = false;
    }
}

void CollisionMgr::PruneCollisionPairCache()
{
    constexpr UINT64 CACHE_KEEP_FRAMES = 120;

    auto iter = m_mapColState.begin();
    while (iter != m_mapColState.end())
    {
        const CollisionPairState& pairState = iter->second;
        
        if (pairState.IsColliding || pairState.LastSeenFrame + CACHE_KEEP_FRAMES >= m_FrameCounter)
        {
            ++iter;
            continue;
        }

        iter = m_mapColState.erase(iter);
    }
}

bool CollisionMgr::IsCollisionDeprecated(const Ptr<CCollider2D>& _LeftCol, const Ptr<CCollider2D>& _RightCol)
{
    // Rect vs Rect OBB Collision testing
    
    if (!_LeftCol->GetOwner()->GetActive() || !_RightCol->GetOwner()->GetActive())               return false;    // Active가 꺼진 오브젝트
    if (_LeftCol->GetOwner()->IsObjectDestroyed() || _RightCol->GetOwner()->IsObjectDestroyed()) return false;    // 곧 삭제 처리될 오브젝트

    Ptr<AMesh> pRectMesh = FIND_ASSET(AMesh, L"RectMesh");

    const Vtx* pVtx             = pRectMesh->GetVtxSysMem();
    const Matrix& matWorldLeft  = _LeftCol->GetWorldMat();
    const Matrix& matWorldRight =_RightCol->GetWorldMat();

    // 월드 공간상에서 충돌을 검사하기 위해서, RectMesh 모델을 각 충돌체의 월드행렬을 곱해서 정점을 충돌체 꼭지점에 배치시킨다.
    // 각 꼭지점끼리 빼서 두 충돌체의 표면 방향벡터를 각 충돌체로부터 2개씩 구한다.
    
    Vec3 Axis[4]{};
    
    // Left collider 축 초기화
    Axis[0] = XMVector3TransformCoord(pVtx[1].vPos, matWorldLeft) - XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);
    Axis[1] = XMVector3TransformCoord(pVtx[3].vPos, matWorldLeft) - XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);

    // Right Collider 축 초기화
    Axis[2] = XMVector3TransformCoord(pVtx[1].vPos, matWorldRight) - XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);
    Axis[3] = XMVector3TransformCoord(pVtx[3].vPos, matWorldRight) - XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);

    Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matWorldRight) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matWorldLeft);

    
    for (int i = 0; i < 4; ++i)
    {
        // 4개의 축 중에서, 하나를 투영 목적지로 정함
        // 원본값을 훼손하면 나중에 투영할 때 문제가 생기기 때문에, 정규화한 벡터를 따로 지역변수로 둠
        Vec3 vProjAxis = Axis[i];
        vProjAxis.Normalize();
        
        // 투영축으로 4개의 벡터를 투영시켜서 얻은 면적의 절반 길이를 구함
        float Dot{};
        
        for (const Vec3& Ax : Axis)
            Dot += fabs(vProjAxis.Dot(Ax));
        
        Dot *= 0.5f;
        
        // 두 충돌체 중심을 잇는 벡터도 투영
        float fCenter = fabs(vCenter.Dot(vProjAxis));
        
        if (fCenter > Dot) return false;
    }
    
    return true;
}

GameObject* CollisionMgr::FindNearestObject(Vec2 _WorldPos, UINT _LayerMask, float* _OutMinDistSq)
{
    const int startX = WorldToCellCoord(_WorldPos.x);
    const int startY = WorldToCellCoord(_WorldPos.y);

    GameObject* nearestObject = nullptr;
    float minDistSq = FLT_MAX;

    // 이미 검사한 콜라이더 ID 추적 (여러 셀에 걸쳐 있는 콜라이더 중복 검사 방지)
    static unordered_set<UINT> setChecked;
    setChecked.clear();

    bool foundAnyInRadius = false;
    
    // 격자 반경을 늘려가며 탐색 (Spiral Search)
    // 맵 크기가 아주 클 수 있으므로 적절한 최대 반경 설정 (예: 50셀 = 9000 유닛)
    const int maxRadius = 50; 
    
    for (int r = 0; r <= maxRadius; ++r)
    {
        bool foundInThisRadius = false;
        
        // 반경 r인 테두리 셀들을 검사
        for (int y = startY - r; y <= startY + r; ++y)
        {
            for (int x = startX - r; x <= startX + r; ++x)
            {
                // 테두리만 검사 (r > 0 일 때)
                if (r > 0 && abs(x - startX) < r && abs(y - startY) < r)
                    continue;

                const ULONGLONG cellKey = MakeCellKey(x, y);

                for (UINT layerIdx = 0; layerIdx < MAX_LAYER; ++layerIdx)
                {
                    if (!(_LayerMask & (1 << layerIdx))) continue;

                    auto it = m_arrLayerBuckets[layerIdx].find(cellKey);
                    if (it == m_arrLayerBuckets[layerIdx].end()) continue;

                    for (UINT colID : it->second)
                    {
                        if (setChecked.find(colID) != setChecked.end()) continue;
                        setChecked.insert(colID);

                        Ptr<CCollider2D> pCol = m_mapColliderByID[colID];
                        if (!pCol || !pCol->GetActive() || pCol->GetOwner()->IsObjectDestroyed() || !pCol->GetOwner()->GetActive())
                            continue;

                        const float distSq = Vec2::DistanceSquared(_WorldPos, pCol->GetOwner()->Transform()->GetWorldPos2D());
                        if (distSq < minDistSq)
                        {
                            minDistSq = distSq;
                            nearestObject = pCol->GetOwner();
                            foundInThisRadius = true;
                            foundAnyInRadius = true;
                        }
                    }
                }
            }
        }

        // 이번 반경에서 무언가를 찾았다면, 
        // 현재까지 찾은 최단 거리가 다음 반경의 최소 거리보다 작다면 확정하고 종료
        if (foundAnyInRadius)
        {
            // 다음 반경 (r+1)의 셀들까지의 최소 거리는 r * m_GridCellSize 이상임
            // (정확히는 r * m_GridCellSize 보다 크지만 보수적으로 계산)
            float nextRadiusMinDist = static_cast<float>(r) * m_GridCellSize;
            if (minDistSq < nextRadiusMinDist * nextRadiusMinDist)
            {
                break;
            }
        }
        
        // 너무 많은 셀을 뒤지는 것을 방지 (어느 정도 찾았으면 종료)
        if (r > 10 && foundAnyInRadius) break;
    }

    // 만약 격자 탐색으로 못 찾았다면 (격자 밖에 있거나 너무 멀리 있는 경우), 해당 레이어 전체 순회 (보조)
    if (!nearestObject)
    {
        for (UINT layerIdx = 0; layerIdx < MAX_LAYER; ++layerIdx)
        {
            if (!(_LayerMask & (1 << layerIdx))) continue;

            for (const auto& pCol : m_arrLayerColliders[layerIdx])
            {
                if (!pCol || !pCol->GetActive() || pCol->GetOwner()->IsObjectDestroyed() || !pCol->GetOwner()->GetActive())
                    continue;

                const float distSq = Vec2::DistanceSquared(_WorldPos, pCol->GetOwner()->Transform()->GetWorldPos2D());
                if (distSq < minDistSq)
                {
                    minDistSq = distSq;
                    nearestObject = pCol->GetOwner();
                }
            }
        }
    }

    if (_OutMinDistSq) *_OutMinDistSq = minDistSq;
    return nearestObject;
}
