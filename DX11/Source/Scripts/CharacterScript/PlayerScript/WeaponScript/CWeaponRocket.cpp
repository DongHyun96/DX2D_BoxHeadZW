#include "pch.h"
#include "CWeaponRocket.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/ProjectileScript/CRocketProjectile.h"

CWeaponRocket::CWeaponRocket()
    : CWeaponScript(SCRIPT_TYPE::WEAPONROCKET)
{
}

CWeaponRocket::~CWeaponRocket()
{
}

bool CWeaponRocket::Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection)
{
    // 아직 발사간격 시간을 다 채우지 못했을 때
    if (GetTimeAfterLastFire() < GetFireIntervalTime()) return false;
    RewindTimeAfterLastFire();

    // 투사체 Spawn 처리 (제대로 Spawn 되지 않으면 (pool 갯수 등의 이유로) false가 반환된다) 
    return GM->SpawnRocketProjectile(ToVec3(_MuzzleWorldPos), _FireDirection, GetDamageAmountPerRound());
}
