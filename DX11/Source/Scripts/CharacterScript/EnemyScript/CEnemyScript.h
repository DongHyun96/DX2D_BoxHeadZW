#pragma once
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

enum class ENEMY_TYPE;
enum class ENEMY_MAINSTATE;

class CEnemyScript : public CCharacterScript
{
protected:

    ENEMY_TYPE m_EnemyType{};
    
private:

    ENEMY_MAINSTATE m_MainState{};
    
public:
    
    CEnemyScript();
    virtual ~CEnemyScript() override;
    CLONE(CEnemyScript)

private:
    
    virtual void Move() override;
    virtual void UpdateCurrentFacedDirection() override;
    virtual void AfterPushedOutFin() override;
    
public:
    
    void SetMainState(ENEMY_MAINSTATE _MainState) { m_MainState = _MainState; }
    ENEMY_MAINSTATE GetMainState() const { return m_MainState; }
    
};
