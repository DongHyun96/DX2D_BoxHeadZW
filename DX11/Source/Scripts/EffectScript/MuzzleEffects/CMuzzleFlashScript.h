#pragma once
#include "Source/Scripts/EffectScript/CFlipbookEffectScript.h"

class CMuzzleFlashScript : public CFlipbookEffectScript
{
private:

    static const float s_OffsetFactorFromMuzzle;
    
private:

    // Player Muzzle인지 체크 (false라면, Turret Muzzle) -> CONTINUE
    bool m_bIsPlayerWeaponMuzzle = true;
    
public:
    
    CMuzzleFlashScript();
    virtual ~CMuzzleFlashScript() override;
    CLONE(CMuzzleFlashScript)

public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    void SetIsPlayerWeaponMuzzle(bool _PlayerMuzzleEffect) { m_bIsPlayerWeaponMuzzle = _PlayerMuzzleEffect; } 
    
private:
    
    void OnDeactivate(const Ptr<GameObject>& _Owner);

};
