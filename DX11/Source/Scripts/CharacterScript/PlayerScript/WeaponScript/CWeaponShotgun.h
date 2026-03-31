#pragma once
#include "CWeaponScript.h"

/// <summary>
/// Num 3 Weapon
/// </summary>
class CWeaponShotgun : public CWeaponScript
{
private:

    // 나가는 방향 사잇각 (5방향으로 나감)
    const float m_FireSpreadAngle;
    
public:

    CWeaponShotgun();
    virtual ~CWeaponShotgun() override;
    CLONE(CWeaponShotgun)

public:
    
    bool Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection) override;
};
