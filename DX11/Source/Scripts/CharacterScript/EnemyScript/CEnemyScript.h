#pragma once
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

class CEnemyScript : public CCharacterScript
{
private:
    
public:
    
    CEnemyScript();
    virtual ~CEnemyScript() override;
    CLONE(CEnemyScript)

private:
    
    virtual void Move() override;
    
};
