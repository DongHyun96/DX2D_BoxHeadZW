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
        vecLayerColliders.clear();
        
        const vector<Ptr<GameObject>>& vecAllObjects = _Level->GetLayer(layerIdx)->GetAllObjects();
        vecLayerColliders.reserve(vecAllObjects.size());
        
        for (const Ptr<GameObject>& object : vecAllObjects)
        {
            Ptr<CCollider2D> pCollider = object->GetCollider2D();
            if (!pCollider) continue;
            
            vecLayerColliders.push_back(pCollider);
            m_mapColliderByID[pCollider->GetEntityInstID()] = pCollider;
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
    
    if (CColliderRect* pRect = dynamic_cast<CColliderRect*>(_Collider.Get()))
    {
        const Vec3 vCenter = pRect->GetWorldPos();
        const Vec3 vWorldScale = pRect->Transform()->GetWorldScale();
        const Vec2 vRectScale = pRect->GetScale();
        const Vec2 vHalfSize = Vec2(vWorldScale.x * vRectScale.x * 0.5f, vWorldScale.y * vRectScale.y * 0.5f);
        
        Vec3 axisX = pRect->Transform()->GetDir(DIR::RIGHT);
        Vec3 axisY = pRect->Transform()->GetDir(DIR::UP);
        axisX.z = 0.f;
        axisY.z = 0.f;

        if (axisX.LengthSquared() > 0.f) axisX.Normalize();
        else axisX = Vec3::Right;

        if (axisY.LengthSquared() > 0.f) axisY.Normalize();
        else axisY = Vec3::Up;

        const float extentX = fabsf(axisX.x) * vHalfSize.x + fabsf(axisY.x) * vHalfSize.y;
        const float extentY = fabsf(axisX.y) * vHalfSize.x + fabsf(axisY.y) * vHalfSize.y;

        _OutMin = Vec2(vCenter.x - extentX, vCenter.y - extentY);
        _OutMax = Vec2(vCenter.x + extentX, vCenter.y + extentY);
        return true;
    }

    if (CColliderCircle* pCircle = dynamic_cast<CColliderCircle*>(_Collider.Get()))
    {
        const Vec3 vCenter = pCircle->GetWorldPos();
        const float fRadius = pCircle->GetRadius();
        
        _OutMin = Vec2(vCenter.x - fRadius, vCenter.y - fRadius);
        _OutMax = Vec2(vCenter.x + fRadius, vCenter.y + fRadius);
        return true;
    }

    if (CColliderPoint* pPoint = dynamic_cast<CColliderPoint*>(_Collider.Get()))
    {
        const Vec3 vCenter = pPoint->GetWorldPos();
        _OutMin = Vec2(vCenter.x, vCenter.y);
        _OutMax = _OutMin;
        return true;
    }

    return false;
}

void CollisionMgr::CollisionBtwLayer(UINT _LeftLayerIdx, UINT _RightLayerIdx)
{
    if (_LeftLayerIdx == _RightLayerIdx)
    {
        CollisionBtwSameLayer(_LeftLayerIdx);
        return;
    }

    const vector<Ptr<CCollider2D>>& vecLeftColliders = m_arrLayerColliders[_LeftLayerIdx];
    const vector<Ptr<CCollider2D>>& vecRightColliders = m_arrLayerColliders[_RightLayerIdx];
    
    if (vecLeftColliders.empty() || vecRightColliders.empty())
        return;

    using CellBuckets = unordered_map<ULONGLONG, vector<UINT>>;

    auto BuildBuckets = [this](const vector<Ptr<CCollider2D>>& _vecColliders, CellBuckets& _OutBuckets)
    {
        _OutBuckets.clear();
        
        for (const Ptr<CCollider2D>& collider : _vecColliders)
        {
            Vec2 vMin{}, vMax{};
            if (!TryGetColliderAABB(collider, vMin, vMax)) continue;

            const int minCellX = WorldToCellCoord(vMin.x);
            const int maxCellX = WorldToCellCoord(vMax.x);
            const int minCellY = WorldToCellCoord(vMin.y);
            const int maxCellY = WorldToCellCoord(vMax.y);

            for (int y = minCellY; y <= maxCellY; ++y)
            {
                for (int x = minCellX; x <= maxCellX; ++x)
                {
                    _OutBuckets[MakeCellKey(x, y)].push_back(collider->GetEntityInstID());
                }
            }
        }
    };

    CellBuckets leftBuckets{};
    CellBuckets rightBuckets{};
    
    BuildBuckets(vecLeftColliders, leftBuckets);
    BuildBuckets(vecRightColliders, rightBuckets);

    const CellBuckets* pIterBuckets = &leftBuckets;
    const CellBuckets* pOtherBuckets = &rightBuckets;
    bool bIterIsLeft = true;
    
    if (leftBuckets.size() > rightBuckets.size())
    {
        pIterBuckets = &rightBuckets;
        pOtherBuckets = &leftBuckets;
        bIterIsLeft = false;
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
                const UINT leftID = bIterIsLeft ? idA : idB;
                const UINT rightID = bIterIsLeft ? idB : idA;
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

void CollisionMgr::CollisionBtwSameLayer(UINT _LayerIdx)
{
    const vector<Ptr<CCollider2D>>& vecLayerColliders = m_arrLayerColliders[_LayerIdx];
    if (vecLayerColliders.size() < 2)
        return;

    using CellBuckets = unordered_map<ULONGLONG, vector<UINT>>;
    CellBuckets buckets{};
    
    for (const Ptr<CCollider2D>& collider : vecLayerColliders)
    {
        Vec2 vMin{}, vMax{};
        if (!TryGetColliderAABB(collider, vMin, vMax)) continue;

        const int minCellX = WorldToCellCoord(vMin.x);
        const int maxCellX = WorldToCellCoord(vMax.x);
        const int minCellY = WorldToCellCoord(vMin.y);
        const int maxCellY = WorldToCellCoord(vMax.y);

        for (int y = minCellY; y <= maxCellY; ++y)
        {
            for (int x = minCellX; x <= maxCellX; ++x)
            {
                buckets[MakeCellKey(x, y)].push_back(collider->GetEntityInstID());
            }
        }
    }

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
    
    const UINT leftID = _LeftCol->GetEntityInstID();
    const UINT rightID = _RightCol->GetEntityInstID();
    const UINT normalizedLeftID = (leftID < rightID) ? leftID : rightID;
    const UINT normalizedRightID = (leftID < rightID) ? rightID : leftID;
    const ULONGLONG pairKey = MakePairKey(leftID, rightID);

    auto [iterState, bInserted] = m_mapColState.try_emplace(pairKey, CollisionPairState{});
    CollisionPairState& pairState = iterState->second;
    if (bInserted)
    {
        pairState.LeftID = normalizedLeftID;
        pairState.RightID = normalizedRightID;
    }
    
    pairState.LastSeenFrame = m_FrameCounter;
    
    if (_LeftCol->IsCollision(_RightCol))
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
