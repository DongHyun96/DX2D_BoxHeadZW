#pragma once
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

class CRunner : public CEnemyScript
{
private:

public:
    CRunner();
    virtual ~CRunner() override;
    CLONE(CRunner)
    
public:
    
    virtual void Begin() override;
    virtual void Tick() override;
};
