#pragma once
#include "CExplosionDome.h"

class CExplosion : public CExplosionDome
{
private:
    
    static const Vec2 s_ScaleSizeBase;
    
private:
    
    float m_UpwardSpeed{};

public:
    CExplosion();
    virtual ~CExplosion() override;
    CLONE(CExplosion)

public:
    
    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    virtual void SetExplosionSize(float _Factor) override;
    
private:
    
    void OnActivateOwnerObject(const Ptr<GameObject>& _OwnerObject);
    
};
