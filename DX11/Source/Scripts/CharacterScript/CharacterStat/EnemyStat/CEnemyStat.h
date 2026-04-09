#pragma once
#include "Source/Scripts/CharacterScript/CharacterStat/CCharacterStat.h"

class CEnemyStat : public CCharacterStat
{
public:
    
    CEnemyStat();
    virtual ~CEnemyStat() override;
    CLONE(CEnemyStat)
    
public:
    
    virtual bool TakeDamage(float _DamageAmount, GameObject* _DamageCauser) override;
    
};
