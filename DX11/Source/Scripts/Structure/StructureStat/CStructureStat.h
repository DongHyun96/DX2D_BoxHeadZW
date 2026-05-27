#pragma once

#include "Source/Scripts/StatScript/CStatScript.h"

class CStructureStat : public CStatScript
{
private:
    
    ExplosionSpawnDesc m_BarrelExplosionDesc{};
    
private:
    
    class CStructureHPBar* m_StructureHPBar{};
    
public:
    
    CStructureStat();
    virtual ~CStructureStat() override;
    CLONE(CStructureStat)

public:

    virtual void Begin() override;
    
public:
    
    virtual bool TakeDamage(float _DamageAmount, GameObject* _DamageCauser) override;
    
};
