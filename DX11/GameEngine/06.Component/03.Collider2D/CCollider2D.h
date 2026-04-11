#pragma once
#include "GameEngine/06.Component/Script/CScript.h"


class CCollider2D;

// CScript 멤버함수여야 하고, Parameter는 다음과 같은 함수 포인터 형식 지정
typedef void(CScript::* COLLISION_EVENT)(CCollider2D*, CCollider2D*);

struct COLLISION_DELEGATE
{
    CScript*        Inst{};
    COLLISION_EVENT MemFunc{};
};

class CCollider2D : public Component
{
    
    friend class CollisionMgr;

private:
    
    bool m_bIsActive = true;
    bool m_Movable = true;
    bool m_bCacheValid = false;
    
    Vec2 m_vCachedMin{};
    Vec2 m_vCachedMax{};
    vector<ULONGLONG> m_vecCachedCellKeys{};
    
private:
    
    Vec2    m_Offset{}; // 부모 오브젝트 위치로부터의 offset
    Vec4    m_Color = Vec4(0.f, 1.f, 0.f, 1.f);
    Matrix  m_matWorld{};

private:
    
    int    m_OverlapCount{};

    vector<COLLISION_DELEGATE> m_vecBeginDelegates{};
    vector<COLLISION_DELEGATE> m_vecOverlapDelegates{};
    vector<COLLISION_DELEGATE> m_vecEndDelegates{};
    
public:
    
    CCollider2D(COMPONENT_TYPE _Type);
    CCollider2D(const CCollider2D& _Origin);
    
    virtual ~CCollider2D() override;

public:
    
    void SetActive(bool _Active) { m_bIsActive = _Active; }
    bool GetActive() const { return m_bIsActive; }
    
public:
    
    void AddDynamicBeginOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);
    void AddDynamicOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);
    void AddDynamicEndOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);

    
protected:
    
    Matrix& GetWorldMat() { return m_matWorld; }
    
public:
    
    GET_SET(Vec2, Offset);
    GET_SET(Vec4, Color);
    GET_SET(bool, Movable);
    void InvalidateCache() { m_bCacheValid = false; }
    bool IsCacheValid() const { return m_bCacheValid; }
    Vec3 GetWorldPos() const { return m_matWorld.Translation(); }

    /// <summary>
    /// 현재 특정 물체와 Overlapping 중인지 
    /// </summary>
    bool IsCurrentlyOverlapping() const { return m_OverlapCount > 0; }

public:

    virtual void FinalTick() override;

private:
    
    void BeginOverlap(const Ptr<CCollider2D>& _Other);
    void Overlap(const Ptr<CCollider2D>& _Other);
    void EndOverlap(const Ptr<CCollider2D>& _Other);
    
    /// <summary>
    /// Collision 여부 검사 범용적으로 처리
    /// </summary>
    /// <param name="_Other"> : 이 충돌체와 충돌검사를 진행할 다른 충돌체 </param>
    /// <returns> : 충돌하는 중이면 return true </returns>
    bool IsCollision(const Ptr<CCollider2D>& _Other);
    
    virtual bool IsCollision(CColliderRect*   _OtherRect)   = 0;
    virtual bool IsCollision(CColliderCircle* _OtherCircle) = 0;
    virtual bool IsCollision(CColliderPoint*  _OtherPoint)  = 0;

public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
