#pragma once
#include "CExplosionDome.h"

class CExplosion : public CExplosionDome
{
private:
    
    static const Vec2 s_ScaleSizeBase;

public:
    CExplosion();
    virtual ~CExplosion() override;

public:
    
    virtual void SetExplosionSize(float _Factor) override;
    
};
