#pragma once
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

enum class ENEMY_TYPE;
enum class ENEMY_STATE;

class CEnemyScript : public CCharacterScript
{
protected:

    ENEMY_TYPE m_EnemyType{};
    
private:

    ENEMY_STATE m_MainState{};
    
public:
    
    CEnemyScript();
    virtual ~CEnemyScript() override;
    CLONE(CEnemyScript)

private:
    
    virtual void Move() override;
    virtual void UpdateCurrentFacedDirection() override;
    
public:
    
    void SetMainState(ENEMY_STATE _MainState) { m_MainState = _MainState; }
    ENEMY_STATE GetMainState() const { return m_MainState; }
    
};
