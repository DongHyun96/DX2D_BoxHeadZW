#pragma once
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

class CMummy : public CEnemyScript
{
private:

public:
    CMummy();
    virtual ~CMummy() override;
    CLONE(CMummy)
    
public:
    
    virtual void Begin() override;
    virtual void Tick() override;
};
