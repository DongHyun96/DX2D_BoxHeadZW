#pragma once
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
    Vec3    Origin{};
    Vec3    Dir{};
    float   MaxDistance{};
};

struct RayCastHit
{
    Ptr<CCollider2D>    Collider{};
    Vec3                Point{};
    Vec3                Normal{};
    float               Distance{};
};

class CollisionMgr : public Singleton<CollisionMgr>
{
    
    SINGLE(CollisionMgr);

private:

    map<ULONGLONG, bool> m_mapColID{};
    
    const float RAY_EPSILON = 0.00001f;
    
public:
    void Progress(const Ptr<ALevel>& _Level);
    
private:
    
    void CollisionBtwLayer(Layer* _Left, Layer* _Right);

    /// <summary>
    /// 같은 레이어 내에서의 충돌검사 처리를 해야할 때
    /// 중복검사 처리를 피함
    /// </summary>
    /// <param name="_Layer"></param>
    void CollisionBtwSameLayer(Layer* _Layer);

private:
    
    /// <summary>
    /// 두 물체의 Collision 검사 진행 & 이전 tick 결과와 현재 tick 검사결과에 따라 Notify 처리 
    /// </summary>
    void CheckCollisionAndNotify(const Ptr<GameObject>& _LeftObject, const Ptr<GameObject>& _RightObject);
    
private:
    
    /// <summary>
    /// (Deprecated) Basic Rect vs Rect OBB Collision testing
    /// </summary>
    bool IsCollisionDeprecated(const Ptr<CCollider2D>& _LeftCol, const Ptr<CCollider2D>& _RightCol);

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
        Vec3&               _OutPoint,
        Vec3&               _OutNormal
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
        const Matrix&   _RectWorldMat,
        float&          _OutT,
        Vec3&           _OutPoint,
        Vec3&           _OutNormal
    );
    
};
