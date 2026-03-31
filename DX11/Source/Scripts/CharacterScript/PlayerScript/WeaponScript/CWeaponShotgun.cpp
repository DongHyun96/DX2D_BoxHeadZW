#include "pch.h"
#include "CWeaponShotgun.h"

#include "GameEngine/03.Manager/08.CollisionMgr/CollisionMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CWeaponShotgun::CWeaponShotgun()
    : CWeaponScript(SCRIPT_TYPE::WEAPONSHOTGUN)
{
}

CWeaponShotgun::~CWeaponShotgun()
{
}

bool CWeaponShotgun::Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection)
{
    // 아직 발사간견 시간을 다 채우지 못했을 때
    if (GetTimeAfterLastFire() < GetFireIntervalTime()) return false;
    RewindTimeAfterLastFire();
    
    Ray2D Ray{};
    Ray.Origin      = _MuzzleWorldPos;
    Ray.Direction   = ToVec3(_FireDirection.Normalized()); 
    Ray.MaxDistance = RESOL_DIAG_LENGTH;
    
    RayCastHit Hit{};
    
    if (CollisionMgr::GetInst()->RayCast(Ray, GetHitScanLayers(), &Hit))
    {
        const Vec3 RayOrigin = ToVec3(Ray.Origin);
        DrawDebugLine(RayOrigin, ToVec3(Hit.Point), DEF_COLOR_RED, 2.f);
        DrawDebugCircle(ToVec3(Hit.Point), 10.f, DEF_COLOR_RED, 2.f);

        // Enemy Hit 판정 (MapObstacle의 경우, Stat이 없음, Character와 설치물 밖에 없다)
        // 검사 허용 Layer 판정에 의해 StatScript를 가지고 있으면 그 StatScript는 Enemy의 것 
        
        Ptr<CCollider2D> CollidedCollider = Hit.Collider;
        if (Ptr<CStatScript> Stat = CollidedCollider->GetOwner()->GetScriptComponent<CStatScript>())
            Stat->TakeDamage(GetDamageAmountPerRound(), _MuzzleWorldPos);
    }
    else
    {
        const Vec3 RayOrigin = ToVec3(Ray.Origin);
        const Vec3 RayDirectionVec3 = ToVec3(Ray.Direction);
        
        DrawDebugLine(RayOrigin, RayOrigin + RayDirectionVec3 * Ray.MaxDistance, DEF_COLOR_GREEN, 2.f);
    }
    
    return true;
}
