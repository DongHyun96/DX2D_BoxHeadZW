#pragma once
#include "CWeaponScript.h"

class CWeaponPistol : public CWeaponScript
{
    
public:
    
    CWeaponPistol();
    virtual ~CWeaponPistol();
    CLONE(CWeaponPistol)
    
public:
    
    virtual bool Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection) override;
    
};
