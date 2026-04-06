#pragma once
#include "Source/Scripts/CharacterScript/CCharacterAnimHandler.h"

class CEnemyAnimHandler : public CCharacterAnimHandler
{
private:
    
    
private:

    class CEnemyScript* m_MainEnemyScript{};
    
private:
    
    ENEMY_MAINSTATE m_PrevMainState = ENEMY_MAINSTATE::END;
    EDIRECTION m_PrevAnimDirection = EDIRECTION::END;
    
public:

    CEnemyAnimHandler();
    virtual ~CEnemyAnimHandler() override;
    CLONE(CEnemyAnimHandler)
    
public:

    virtual void Init() override;    
    virtual void Begin() override;

private:
    
    virtual void UpdateAnimTransition() override;

    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
