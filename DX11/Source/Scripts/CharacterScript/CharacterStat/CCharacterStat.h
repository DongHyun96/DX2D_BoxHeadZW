#pragma once

#include "Source/Scripts/StatScript/CStatScript.h"

class CCharacterStat : public CStatScript
{
public:
    
    CCharacterStat();
    virtual ~CCharacterStat() override;
    CLONE(CCharacterStat)
    
protected:
    
    CCharacterStat(enum SCRIPT_TYPE _ScriptType);
    
public:
    
    virtual bool TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos) override;
    
};
