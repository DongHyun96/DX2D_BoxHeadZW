#pragma once
#include <array>
#include <unordered_map>
#include <unordered_set>
#include "GameEngine/04.Asset/04.Level/ALevel.h"

union COL_ID
{
    struct
    {
        UINT LeftID;
        UINT RightID;
    };
    ULONGLONG ID;
};

struct Ray2D
{
    Vec2    Origin{};
    Vec2    Direction{};
    float   MaxDistance{};
};

struct RayCastHit
{
    Ptr<CCollider2D>    Collider{};
    Vec2                Point{};
    Vec2                Normal{};
    float               Distance{};
};

class CollisionMgr : public Singleton<CollisionMgr>
{
    
    SINGLE(CollisionMgr);

private:

    using CellBuckets = unordered_map<ULONGLONG, vector<UINT>>;

    struct CollisionPairState
    {
        bool    IsColliding{};
        UINT64  LastSeenFrame{};
        UINT    LeftID{};
        UINT    RightID{};
    };

    unordered_map<ULONGLONG, CollisionPairState>        m_mapColState{};
    array<vector<Ptr<CCollider2D>>, MAX_LAYER>          m_arrLayerColliders{};
    array<CellBuckets, MAX_LAYER>                       m_arrLayerBuckets{};
    unordered_map<UINT, Ptr<CCollider2D>>               m_mapColliderByID{};
    UINT64                                              m_FrameCounter{};
    float                                               m_GridCellSize = 180.f;
    
    const float RAY_EPSILON = 0.00001f;
    
public:
    void SetGridCellSize(float _Size);
    float GetGridCellSize() const { return m_GridCellSize; }
    
    /// <summary>
    /// 현재 레벨에 배치된 콜라이더들의 크기를 샘플링하여 최적의 격자 크기를 계산하고 설정
    /// </summary>
    /// <param name="_Level"></param>
    void CalculateOptimalGridCellSize(const Ptr<ALevel>& _Level);

    void Progress(const Ptr<ALevel>& _Level);
    
    void OnLevelPlayToStop()
    {
        m_mapColState.clear();
        m_mapColliderByID.clear();
        for (auto& vecLayerColliders : m_arrLayerColliders)
            vecLayerColliders.clear();
        for (auto& layerBuckets : m_arrLayerBuckets)
            layerBuckets.clear();
        m_FrameCounter = 0;
    }
    
    void OnLevelStopToPlay()
    {
        m_mapColState.clear();
        m_mapColliderByID.clear();
        for (auto& vecLayerColliders : m_arrLayerColliders)
            vecLayerColliders.clear();
        for (auto& layerBuckets : m_arrLayerBuckets)
            layerBuckets.clear();
        m_FrameCounter = 0;
    }
    
    void OnLevelChanged(ALevel* _PrevLevel, ALevel* _NextLevel)
    {
        m_mapColState.clear();
        m_mapColliderByID.clear();
        for (auto& vecLayerColliders : m_arrLayerColliders)
            vecLayerColliders.clear();
        for (auto& layerBuckets : m_arrLayerBuckets)
            layerBuckets.clear();
        m_FrameCounter = 0;
    }
    
private:
    
    void BuildLayerColliderCache(const Ptr<ALevel>& _Level);
    void CollisionBtwLayer(UINT _LeftLayerIdx, UINT _RightLayerIdx);

    /// <summary>
    /// 같은 레이어 내에서의 충돌검사 처리를 해야할 때
    /// 중복검사 처리를 피함
    /// </summary>
    /// <param name="_Layer"></param>
    void CollisionBtwSameLayer(UINT _LayerIdx);
    void ProcessStaleCollisionPairs();
    void PruneCollisionPairCache();
    
    bool TryGetColliderAABB(const Ptr<CCollider2D>& _Collider, Vec2& _OutMin, Vec2& _OutMax);
    int WorldToCellCoord(float _Coord) const;
    ULONGLONG MakeCellKey(int _CellX, int _CellY) const;
    ULONGLONG MakePairKey(UINT _A, UINT _B) const;

private:
    
    /// <summary>
    /// 두 물체의 Collision 검사 진행 & 이전 tick 결과와 현재 tick 검사결과에 따라 Notify 처리 
    /// </summary>
    void CheckCollisionAndNotify(const Ptr<CCollider2D>& _LeftCol, const Ptr<CCollider2D>& _RightCol);
    
private:
    
    /// <summary>
    /// (Deprecated) Basic Rect vs Rect OBB Collision testing
    /// </summary>
    bool IsCollisionDeprecated(const Ptr<CCollider2D>& _LeftCol, const Ptr<CCollider2D>& _RightCol);

    
    
    
    
    /* Collision 관련 Utility 기능 */
    
public:
    
    /// <summary>
    /// RayCast 충돌검사
    /// </summary>
    /// <param name="_Ray"> Origin + Dir + MaxDistance </param>
    /// <param name="_TargetLayerMask"> 검사할 레이어 비트마스크 </param>
    /// <param name="_OutHit"> 히트 정보를 받을 포인터(옵션) </param>
    /// <param name="_Ignore"> 무시할 콜라이더(옵션) </param>
    /// <returns> 충돌했다면 return true </returns>
    bool RayCast
    (
        const Ray2D&        _Ray,
        UINT                _TargetLayerMask,
        RayCastHit*         _OutHit  = nullptr,
        const CCollider2D*  _Ignore = nullptr
    );


    /// <summary>
    /// RayCast 충돌검사
    /// </summary>
    /// <param name="_Ray"> Origin + Dir + MaxDistance </param>
    /// <param name="_vecTargetLayerMask"> 검사할 레이어 비트마스크 vector </param>
    /// <param name="_OutHit"> 히트 정보를 받을 포인터(옵션) </param>
    /// <param name="_Ignore"> 무시할 콜라이더(옵션) </param>
    /// <returns> 충돌했다면 return true </returns>
    bool RayCast
    (
        const Ray2D&        _Ray,
        const vector<UINT>& _vecTargetLayerMask,
        RayCastHit*         _OutHit  = nullptr,
        const CCollider2D*  _Ignore = nullptr
    );
    
    bool RayCast
    (
        const Ray2D&        _Ray,
        const set<UINT>&    _setTargetLayerMask,
        RayCastHit*         _OutHit  = nullptr,
        const CCollider2D*  _Ignore = nullptr
    );
    
    GameObject* FindNearestObject(Vec2 _WorldPos, UINT _LayerMask, float* _OutMinDistSq = nullptr);
    
private:
    /// <summary>
    /// TargetLayerMask에서의 Hit 했는지의 판정과, BestHit 찾기
    /// </summary>
    /// <param name="_TargetLayerMask"> : 검사할 레이어 비트마스크 </param>
    /// <param name="_Ray"></param>
    /// <param name="_bOutFoundHit"></param>
    /// <param name="_OutBestHit"></param>
    /// <param name="_Ignore"></param>
    void RayCastFindBestHit
    (
        UINT                _TargetLayerMask,
        const Ray2D&        _Ray,
        bool&               _bOutFoundHit,
        RayCastHit&         _OutBestHit,
        const CCollider2D*  _Ignore
    );
    
private:
    /// <summary>
    /// Circle과 Ray 충돌 검사
    /// </summary>
    /// <param name="_Ray"></param>
    /// <param name="_Circle"></param>
    /// <param name="_OutT"> : Hit 거리 </param>
    /// <param name="_OutPoint"></param>
    /// <param name="_OutNormal"></param>
    /// <returns></returns>
    bool RayVsCircle
    (
        const Ray2D&        _Ray,
        CColliderCircle*    _Circle,
        float&              _OutT,
        Vec2&               _OutPoint,
        Vec2&               _OutNormal
    );

    /// <summary>
    /// Rect와 Ray 충돌 검사
    /// </summary>
    /// <param name="_Ray"></param>
    /// <param name="_RectWorldMat"></param>
    /// <param name="_OutT"> : Hit 거리 </param>
    /// <param name="_OutPoint"></param>
    /// <param name="_OutNormal"></param>
    /// <returns></returns>
    bool RayVsRectOBB
    (
        const Ray2D&    _Ray,
        CColliderRect*  _Rect,
        float&          _OutT,
        Vec2&           _OutPoint,
        Vec2&           _OutNormal
    );
    
};
