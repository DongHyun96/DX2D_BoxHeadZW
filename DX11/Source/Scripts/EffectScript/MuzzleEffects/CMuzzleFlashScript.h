#pragma once
#include "Source/Scripts/EffectScript/CFlipbookEffectScript.h"

class CMuzzleFlashScript : public CFlipbookEffectScript
{
private:

    static const float s_OffsetFactorFromMuzzle;
    
private:

    // Player Muzzle인지 체크 (false라면, Turret Muzzle) -> CONTINUE
    bool m_bIsPlayerWeaponMuzzle = true;
    
    class CPlayerWeaponHandler* m_WeaponHandler{};
    
public:
    
    CMuzzleFlashScript();
    virtual ~CMuzzleFlashScript() override;
    CLONE(CMuzzleFlashScript)

public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;
    
public:
    
    void SetIsPlayerWeaponMuzzle(bool _PlayerMuzzleEffect) { m_bIsPlayerWeaponMuzzle = _PlayerMuzzleEffect; } 
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

};
