#include "pch.h"
#include "TurretAttackStrategy.h"

#include "CTurret.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/08.CollisionMgr/CollisionMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/PlayerScript/StructureHandler/CStructureHandler.h"
#include "Source/Scripts/EffectScript/MuzzleEffects/CMuzzleFlashScript.h"
#include "Source/Scripts/StatScript/CStatScript.h"

float Turret_MGAttackStrategy::s_BurstInterval = 0.1f;

void Turret_MGAttackStrategy::WaitAttack()
{
    // 사격 요청이 들어왔을 때 첫 발 바로 사격 가능하도록 처리
    m_BurstFireTimer = s_BurstInterval;
    m_FireCount = 0;
}

bool Turret_MGAttackStrategy::UseAttackStrategy(CTurret* _Turret, GameObject* _Target)
{
    m_BurstFireTimer += DT;
    
    if (m_BurstFireTimer < s_BurstInterval) return false;
    m_BurstFireTimer = 0.f;

    // 점사 대기 시간 끝, 사격 처리
    
    /////////// 사격 Sound 재생 ///////////
    // 사격 소리가 3가지가 있음
    const int randomSuffix = GetRandom(1, 3);
    wstring SoundKey = L"Sound\\MGShot" + to_wstring(randomSuffix) + L".wav";
    Ptr<ASound> ShootingSound = FIND_ASSET(ASound, SoundKey);
    ShootingSound->Play(1, 0.75f, true);

    // 현재 Muzzle Pos World 위치
    const Vec2 MuzzlePos2D = ToVec2(_Turret->FlipbookRender()->GetCurrentSpritePinPointToWorldPos());
    const Vec2 TurretToTarget = _Target->Transform()->GetWorldPos2D() - MuzzlePos2D;

    /////////// 피격 처리 ///////////
    // Target을 향하는 방향으로 Ray Casting
    Ray2D Ray{};
    Ray.Origin      = MuzzlePos2D;
    Ray.Direction   = TurretToTarget.Normalized(); 
    Ray.MaxDistance = TurretToTarget.Length();
    
    RayCastHit Hit{};

    if (CollisionMgr::GetInst()->RayCast(Ray, CStructureHandler::GetTurretHitScanLayers(), &Hit))
    {
        Ptr<CCollider2D> CollidedCollider = Hit.Collider;
        if (Ptr<CStatScript> Stat = CollidedCollider->GetOwner()->GetScriptComponent<CStatScript>())
            Stat->TakeDamage(GetRandom(20.f, 35.f), _Turret->GetOwner());
    }
    
    /////////// Muzzle Effect ///////////
    
    GameObject* SpawnedFlashEffect = _Turret->PoolComponent()->SpawnObject(ToVec3(MuzzlePos2D)); 
    if (SpawnedFlashEffect)
    {
        SpawnedFlashEffect->FlipbookRender()->Play(0, 15.f, 1);
        // _Turret->GetOwner()->AddChild(SpawnedFlashEffect);
    }

    // 3회 사격이 끝나면 이번 턴 공격 종료
    return ++m_FireCount >= 3;
}

bool Turret_MortarAttackStrategy::UseAttackStrategy(CTurret* _Turret, GameObject* _Target)
{
    const Vec3 MuzzlePinPointWorldPos = _Turret->FlipbookRender()->GetCurrentSpritePinPointToWorldPos();
    
    const Vec2 MuzzlePos2D = ToVec2(MuzzlePinPointWorldPos);
    const Vec2 TargetPos2D = _Target->Transform()->GetWorldPos2D();
    
    // 방향과 거리 계산
    Vec2 TurretToTarget = TargetPos2D - MuzzlePos2D;
    float Distance = TurretToTarget.Length();
    TurretToTarget.Normalize();
    
    // 물리 상수 세팅
    const float Gravity = 980.f; // CGrenade::s_Gravity

    // 고정 체공 시간 방식
    const float HangTime = 1.f; // 어디서 쏘든 무조건 1.5초 뒤에 명중
    const float ThrowSpeedXY = Distance / HangTime;
    const float MuzzleZ = 1.f;
    const float UpwardSpeed = (Gravity * HangTime / 2.0f) - (MuzzleZ / HangTime);

    GM->SpawnGrenade
    (
        MuzzlePinPointWorldPos, 
        TurretToTarget, 
        GetRandom(50.f, 120.f), 
        0,              
        ThrowSpeedXY, 
        UpwardSpeed, 
        false, 
        false
    );
    
    return true;
}

bool Turret_RocketAttackStrategy::UseAttackStrategy(CTurret* _Turret, GameObject* _Target)
{
    // 현재 Muzzle Pos World 위치
    const Vec3 MuzzlePinPointWorldPos = _Turret->FlipbookRender()->GetCurrentSpritePinPointToWorldPos(); 
    const Vec2 MuzzlePos2D = ToVec2(MuzzlePinPointWorldPos);
    const Vec2 TurretToTarget = _Target->Transform()->GetWorldPos2D() - MuzzlePos2D;
    
    GM->SpawnRocketProjectile(MuzzlePinPointWorldPos, TurretToTarget, GetRandom(75.f, 120.f));
    return true;
}
