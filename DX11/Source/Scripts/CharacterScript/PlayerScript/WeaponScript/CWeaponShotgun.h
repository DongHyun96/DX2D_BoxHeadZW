#pragma once
#include "CWeaponScript.h"

/// <summary>
/// Num 3 Weapon
/// </summary>
class CWeaponShotgun : public CWeaponScript
{
public:

    CWeaponShotgun();
    virtual ~CWeaponShotgun() override;
    CLONE(CWeaponShotgun)
    
public:
    
    bool Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection) override;
};
