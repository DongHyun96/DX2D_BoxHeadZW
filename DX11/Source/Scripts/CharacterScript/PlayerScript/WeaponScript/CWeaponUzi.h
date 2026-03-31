#pragma once
#include "CWeaponScript.h"

/// <summary>
/// Num 2 Weapon
/// </summary>
class CWeaponUzi : public CWeaponScript
{
public:
    
    CWeaponUzi();
    virtual ~CWeaponUzi();
    CLONE(CWeaponUzi)
    
public:
    
    virtual bool Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection) override;
    
};
