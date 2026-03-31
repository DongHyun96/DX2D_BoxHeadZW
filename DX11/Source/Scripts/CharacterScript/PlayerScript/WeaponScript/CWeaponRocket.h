#pragma once
#include "CWeaponScript.h"

/// <summary>
/// Num 5 Weapon
/// </summary>
class CWeaponRocket : public CWeaponScript
{
public:
    
    CWeaponRocket();
    virtual ~CWeaponRocket() override;
    CLONE(CWeaponRocket)    
public:
    
    virtual bool Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection) override;
    
};
