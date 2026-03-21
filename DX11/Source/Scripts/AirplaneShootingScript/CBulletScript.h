#pragma once
#include "GameEngine/06.Component/Script/CScript.h"

class CBulletScript : public CScript
{
private:
    
    Ptr<GameObject> m_Target{};
    
private:
    
    float m_Speed    = 5.f;
    float m_TimeAfterSpawned{}; // Spawn 처리된 이후로 지난 시간
    
public:
    
    CBulletScript();
    virtual ~CBulletScript() override;

public:
    
    virtual void Begin() override;
    
    void Tick() override;
    
    void BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void Overlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void EndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
    
    CLONE(CBulletScript)
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
    
public:

    SET(Ptr<GameObject>, Target);
    
    bool Fire(const Vec3& _StartPosition, const Vec3& _FireDirection, float _Speed);
        
    
    
};
