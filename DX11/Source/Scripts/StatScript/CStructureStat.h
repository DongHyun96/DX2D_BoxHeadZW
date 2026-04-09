#pragma once
#include "CStatScript.h"

class CStructureStat : public CStatScript
{
private:
    
    ExplosionSpawnDesc m_BarrelExplosionDesc{};
    
public:
    
    CStructureStat();
    virtual ~CStructureStat() override;
    CLONE(CStructureStat)
    
    
public:
    
    virtual bool TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos) override;
    
};
