#pragma once
#include "CExplosionDome.h"

class CExplosion : public CExplosionDome
{
private:
    
    static const Vec2 s_ScaleSizeBase;
    
private:
    
    Vec2 m_UpwardVelocity{};
    bool m_UseCollisionForDamaging = true; // Collision 검사로 들어온 Character들에게 Damage를 줄건지
    
private:

    // 누적 시간 측정
    float m_Age{};
    const float m_MaxLifeTime = 3.f;

public:
    CExplosion();
    virtual ~CExplosion() override;
    CLONE(CExplosion)

public:
    
    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    virtual void SetExplosionSize(float _Factor) override;
    void SetUseCollisionForDamaging(bool _UseCollisionForDamaging) { m_UseCollisionForDamaging = _UseCollisionForDamaging; }
    void SetUpwardVelocity(const Vec2& _UpwardVelocity) { m_UpwardVelocity = _UpwardVelocity; }
    
private:
    
    void OnActivateOwnerObject(const Ptr<GameObject>& _OwnerObject);
    
};
