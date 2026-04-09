#include "pch.h"
#include "CFlameLineHandler.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CFlameLineHandler::CFlameLineHandler()
    : CScript(SCRIPT_TYPE::FLAMELINEHANDLER)
{
}

CFlameLineHandler::~CFlameLineHandler()
{
}

void CFlameLineHandler::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CFlameLineHandler::OnAttackColliderBeginOverlap);
}

void CFlameLineHandler::Tick()
{
    if (!ColliderRect()->GetActive()) return;
    
    m_AttackColliderScaleTimer += DT;
    ColliderRect()->SetScaleX(MappingToNewRange(m_AttackColliderScaleTimer, 0.f, 1.f, 1.f, 10.f));
    
    static const float FirePillarIntervalTime = 0.05f; // 이 간격으로 ColliderRect의 오른쪽 모서리 위치에 FirePillar 스폰 처리
    m_PillarIntervalTimer += DT;
    
    if (m_PillarIntervalTimer > FirePillarIntervalTime) // Time to spawn Pillar 
    {
        m_PillarIntervalTimer -= FirePillarIntervalTime;
        const Vec3& ColliderRTWorldPos = ColliderRect()->GetCornerWorldPos(1);
        const Vec3& ColliderRBWorldPos = ColliderRect()->GetCornerWorldPos(2);
        
        const Vec2 Direction = ToVec2(ColliderRBWorldPos - ColliderRTWorldPos);
        // Vec2 FinalPos = ColliderRBWorldPos - 
        
        GM->SpawnFirePillar(ColliderRBWorldPos, 0.f);
    }

    // 총 공격 시간 끝이라고 판단 처리
    if (m_AttackColliderScaleTimer > 2.f)
        ColliderRect()->SetActive(false);
}

void CFlameLineHandler::MakeFlameLine(float _Angle, float _Damage)
{
    ColliderRect()->SetActive(true);
    // 총 2초 동안 다 늘어남
    // AttackCollider의 경우 2초보다 더 나중에 꺼야함
    
    m_Damage = _Damage;
    Transform()->SetRelativeRotZ(_Angle);
    ColliderRect()->SetScaleX(1.f);
    
    m_AttackColliderScaleTimer = 0.f;
}

void CFlameLineHandler::OnAttackColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    if (const Ptr<CStatScript>& Stat = _OtherCollider->GetOwner()->GetScriptComponent<CStatScript>())
        Stat->TakeDamage(m_Damage, ToVec2(ColliderRect()->GetWorldPos()));
}
