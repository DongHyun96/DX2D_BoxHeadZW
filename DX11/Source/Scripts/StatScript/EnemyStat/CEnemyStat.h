#pragma once
#include "Source/Scripts/StatScript/CStatScript.h"

class CEnemyStat : public CStatScript
{
public:
    
    CEnemyStat();
    virtual ~CEnemyStat() override;
    CLONE(CEnemyStat)
    
public:
    
    virtual bool TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos) override;
    
};
