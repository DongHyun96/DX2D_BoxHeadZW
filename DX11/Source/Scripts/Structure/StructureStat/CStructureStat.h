#pragma once

#include "Source/Scripts/StatScript/CStatScript.h"

class CStructureStat : public CStatScript
{
private:
    
    ExplosionSpawnDesc m_BarrelExplosionDesc{};
    
public:
    
    CStructureStat();
    virtual ~CStructureStat() override;
    CLONE(CStructureStat)
    
    
public:
    
    virtual bool TakeDamage(float _DamageAmount, GameObject* _DamageCauser) override;
    
};
