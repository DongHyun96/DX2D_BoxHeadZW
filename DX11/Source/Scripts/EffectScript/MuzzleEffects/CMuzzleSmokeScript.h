#pragma once
#include "Source/Scripts/EffectScript/CFlipbookEffectScript.h"

class CMuzzleSmokeScript : public CFlipbookEffectScript
{
private:
    
public:
    
    CMuzzleSmokeScript();
    virtual ~CMuzzleSmokeScript() override;
    CLONE(CMuzzleSmokeScript)

public:

    virtual void Begin() override;
    
};
