#pragma once
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

class CDevil : public CEnemyScript
{
private:

public:
    CDevil();
    virtual ~CDevil() override;
    CLONE(CDevil)
    
public:
    
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    void OnAttackFlipbookEndNotify() override;
    void HandleStateTransition() override;
    
};
