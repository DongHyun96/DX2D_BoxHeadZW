#pragma once
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

class CVampire : public CEnemyScript
{
private:

public:
    CVampire();
    virtual ~CVampire() override;
    CLONE(CVampire)
    
public:
    
    virtual void Begin() override;
    virtual void Tick() override;
};
