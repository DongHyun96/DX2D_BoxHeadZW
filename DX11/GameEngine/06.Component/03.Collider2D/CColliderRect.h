#pragma once
#include "CCollider2D.h"

class CColliderRect : public CCollider2D
{
    
    friend class CColliderCircle;
    friend class CColliderPoint;
    
private:

    Vec2 m_PivotLocal{};
    Vec2 m_Scale = {1.f, 1.f};
    
    // Circle과 충돌 되었을 때 기록된, Circle과 가장 가까운 점
    Vec3 m_CircleClosestPointContacted{};
    
private:
    
    Vec3 m_CornersWorldPos[4]; // LT, RT, RB, LB
    Vec3 m_EdgeMid[4]; // 각 변의 중점 (Top, Right, Bottom, Left 순)

public:
    
    CColliderRect();
    virtual ~CColliderRect() override;
    
    CLONE(CColliderRect)
    
    virtual void FinalTick() override;
    
    
public:
    
    GET_SET(Vec2, Scale)
    GET_SET(Vec2, PivotLocal)
    
    const Vec3& GetCornerWorldPos(UINT _CornerIdx) const { return m_CornersWorldPos[_CornerIdx]; }
    const Vec3& GetEdgeMidWorldPos(UINT _Idx) const { return m_EdgeMid[_Idx]; }
    
    void SetScaleX(float _ScaleX) { m_Scale.x = _ScaleX; }
    void SetScaleY(float _ScaleY) { m_Scale.y = _ScaleY; }

private:
    
    virtual bool IsCollision(CColliderRect*     _OtherRect)     override;
    virtual bool IsCollision(CColliderCircle*   _OtherCircle)   override;
    virtual bool IsCollision(CColliderPoint*    _OtherPoint)   override;
    
private:
    
    bool AABBCollision(CColliderRect*   _OtherRect);
    bool AABBCollision(CColliderCircle* _OtherCircle);
    bool AABBCollision(CColliderPoint*  _OtherPoint);
    
    bool OBBCollision(CColliderRect*    _OtherRect);
    bool OBBCollision(CColliderCircle*  _OtherCircle);
    bool OBBCollision(CColliderPoint*   _OtherPoint);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
public:
    
    Vec3 GetCircleClosestPointContacted() const { return m_CircleClosestPointContacted; }
    
};
