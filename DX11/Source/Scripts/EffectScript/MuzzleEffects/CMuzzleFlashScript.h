#pragma once
#include "Source/Scripts/EffectScript/CFlipbookEffectScript.h"

class CMuzzleFlashScript : public CFlipbookEffectScript
{
private:

    static const float s_OffsetFactorFromMuzzle;
    
private:
    
public:
    
    CMuzzleFlashScript();
    virtual ~CMuzzleFlashScript() override;
    CLONE(CMuzzleFlashScript)

public:
    
    virtual void Tick() override;

};
