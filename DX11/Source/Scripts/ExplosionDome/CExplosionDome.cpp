#include "pch.h"
#include "CExplosionDome.h"

#include "Source/ScriptMgr.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CExplosionDome::CExplosionDome()
    : CScript(SCRIPT_TYPE::EXPLOSIONDOME)
{
}

CExplosionDome::~CExplosionDome()
{
}

void CExplosionDome::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CExplosionDome::BeginOverlap);
}

void CExplosionDome::Tick()
{
}

void CExplosionDome::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // 이미 Damage를 준 Collider가 들어왔을 때
    if (m_setAlreadyDamaged.contains(_OtherCollider)) return;
    
    if (Ptr<CStatScript> Stat = _OtherCollider->GetOwner()->GetScriptComponent<CStatScript>())
    {
        // 두 번 Damage 처리 x
        m_setAlreadyDamaged.insert(_OtherCollider);
        Stat->TakeDamage(m_DamageAmount, ToVec2(Transform()->GetWorldPos()));
    }
}
