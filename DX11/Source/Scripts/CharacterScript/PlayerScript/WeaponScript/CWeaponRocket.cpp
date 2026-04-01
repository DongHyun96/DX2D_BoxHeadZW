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

    // 투사체 Spawn
    if (CPoolComponent* Pooler = GM->GetRocketProjectilePooler())
    {
        GameObject* SpawnedRocketObj = Pooler->SpawnObject(ToVec3(_MuzzleWorldPos));
        if (!SpawnedRocketObj) return false; // 제대로 Projectile 스폰 처리가 안되었음
        
        Ptr<CRocketProjectile> ProjectileScript = SpawnedRocketObj->GetScriptComponent<CRocketProjectile>();
        ProjectileScript->SetDirection(_FireDirection.Normalized());
        ProjectileScript->SetDamage(GetDamageAmountPerRound());
        
        SpawnedRocketObj->FlipbookRender()->Play(0, 15.f, -1);
    }
    
    // Sound play
    Ptr<ASound> Sound = FIND_ASSET(ASound, L"Sound\\RocketShot.mp3");
    Sound->Play(1, 0.5f, true);
    
    return true;
}
