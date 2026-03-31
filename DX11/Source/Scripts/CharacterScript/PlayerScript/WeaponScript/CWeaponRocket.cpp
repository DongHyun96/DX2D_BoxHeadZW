#include "pch.h"
#include "CWeaponRocket.h"

#include "Source/ScriptMgr.h"

CWeaponRocket::CWeaponRocket()
    : CWeaponScript(SCRIPT_TYPE::WEAPONROCKET)
{
}

CWeaponRocket::~CWeaponRocket()
{
}

bool CWeaponRocket::Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection)
{
    return false;
}
