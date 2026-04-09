#include "pch.h"
#include "CExplosionDome.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/ProjectileScript/CGrenade.h"
#include "Source/Scripts/StatScript/CStatScript.h"
#include "Source/Scripts/Structure/CBarrel.h"

const Vec2 CExplosionDome::s_ScaleSizeBase = { 150.f, 100.f }; 

CExplosionDome::CExplosionDome()
    : CScript(SCRIPT_TYPE::EXPLOSIONDOME)
{
}

CExplosionDome::~CExplosionDome()
{
}

CExplosionDome::CExplosionDome(SCRIPT_TYPE _ScriptType)
    : CScript(_ScriptType)
{
}

void CExplosionDome::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CExplosionDome::BeginOverlap);
}

void CExplosionDome::Tick()
{
}

void CExplosionDome::SetExplosionSize(float _Factor)
{
    Transform()->SetRelativeScaleXY(s_ScaleSizeBase * _Factor);
}

void CExplosionDome::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // TODO : Damage를 직접 입히는 ExplosionDome 종류가 아닐수도 있음(AirStrike)
    
    // 이미 Damage를 준 Collider가 들어왔을 때
    if (m_setAlreadyDamaged.contains(_OtherCollider)) return;

    Ptr<CStatScript> Stat = _OtherCollider->GetOwner()->GetScriptComponent<CStatScript>();
    
    if (Ptr<CBarrel> Barrel = _OtherCollider->GetOwner()->GetScriptComponent<CBarrel>())
    {
        // Grenade일 경우에만 예외적으로 터트리기 처리
        if (m_SpawnedBy && dynamic_cast<CGrenade*>(m_SpawnedBy))
        {
            // 두 번 Damage 처리 x
            m_setAlreadyDamaged.insert(_OtherCollider);
            Stat->TakeDamage(m_DamageAmount, GetOwner());
        }
        return;
    }

    // Normal case
    if (Stat)
    {
        // 두 번 Damage 처리 x
        m_setAlreadyDamaged.insert(_OtherCollider);
        Stat->TakeDamage(m_DamageAmount, GetOwner());
    }
}
