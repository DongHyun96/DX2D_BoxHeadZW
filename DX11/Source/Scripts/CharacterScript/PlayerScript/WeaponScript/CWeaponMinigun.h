#pragma once
#include "CWeaponScript.h"

/// <summary>
/// Num 4 Weapon
/// </summary>
class CWeaponMinigun : public CWeaponScript
{
public:
    
    CWeaponMinigun();
    virtual ~CWeaponMinigun() override;
    CLONE(CWeaponMinigun)
    
public:
    
    bool Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection) override;
};
