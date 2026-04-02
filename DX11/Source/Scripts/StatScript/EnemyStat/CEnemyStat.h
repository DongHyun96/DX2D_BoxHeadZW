#pragma once
#include "Source/Scripts/StatScript/CCharacterStat.h"

class CEnemyStat : public CCharacterStat
{
public:
    
    CEnemyStat();
    virtual ~CEnemyStat() override;
    CLONE(CEnemyStat)
    
public:
    
    virtual bool TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos) override;
    
};
