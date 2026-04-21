#include "pch.h"
#include "CFlameLineHandler.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/StatScript/CStatScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"

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
    
    static constexpr float TotalAttackTime              = 0.75f;
    static constexpr float FirePillarIntervalTime       = 0.055f; // 이 간격으로 ColliderRect의 오른쪽 모서리 위치에 FirePillar 스폰 처리
    static constexpr float FirePillarSpawnDownOffsetY   = 18.f; // 우측 변 중앙에서 아래로 내릴 오프셋
    static constexpr float AttackColliderScaleDest      = 9.5f; // 얼만큼 AttackCollider 늘릴것인지
    
    
    m_AttackColliderScaleTimer += DT;
    ColliderRect()->SetScaleX(MappingToNewRange(m_AttackColliderScaleTimer, 0.f, TotalAttackTime, 1.f, AttackColliderScaleDest));
    
    m_PillarIntervalTimer += DT;
    
    if (m_PillarIntervalTimer > FirePillarIntervalTime) // Time to spawn Pillar 
    {
        m_PillarIntervalTimer -= FirePillarIntervalTime;
        Vec3 SpawnPos = ColliderRect()->GetEdgeMidWorldPos(1);
        SpawnPos.y -= FirePillarSpawnDownOffsetY;
        SpawnPos.z = SpawnPos.y;
        
        if (GM->SpawnFirePillar(SpawnPos, 0.f))
            GM->GetBackgroundCellManager()->SpawnScorchDecal(ToVec2(SpawnPos), Vec2::One * GetRandom(3.f, 6.f));
    }

    // 총 공격 시간 끝이라고 판단 처리
    if (m_AttackColliderScaleTimer > TotalAttackTime)
    {
        ColliderRect()->SetActive(false);
        InitWaitState();
    }
}

void CFlameLineHandler::InitSpawn()
{
    GetOwner()->SetActive(true);
    InitWaitState();
}

void CFlameLineHandler::InitWaitState()
{
    m_AttackColliderScaleTimer = 0.f;
    m_PillarIntervalTimer      = 0.f;
    m_PlayerDamagedCount       = 0;
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
    {
        if (_OtherCollider->GetOwner()->GetScriptComponent<CPlayerScript>())
        {
            if (m_PlayerDamagedCount < 3) // Player의 경우 한 턴에 3회 이상 공격을 하지 못하도록 처리를 한다
            {
                Stat->TakeDamage(m_Damage, GetOwner()->GetParent());
                m_PlayerDamagedCount++;
            }
        }
        else Stat->TakeDamage(120.f, GetOwner()->GetParent()); // 구조물의 경우, 더 많은 양의 데미지를 입히도록 처리한다
    }
}
