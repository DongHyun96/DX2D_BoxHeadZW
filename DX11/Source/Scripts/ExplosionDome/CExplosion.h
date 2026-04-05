#pragma once
#include "CExplosionDome.h"

class CExplosion : public CExplosionDome
{
private:
    
    static const Vec2 s_ScaleSizeBase;
    
private:
    
    float m_UpwardSpeed{};
    bool m_UseCollisionForDamaging = true; // Collision 검사로 들어온 Character들에게 Damage를 줄건지

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
    
private:
    
    void OnActivateOwnerObject(const Ptr<GameObject>& _OwnerObject);
    
};
