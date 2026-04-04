#include "pch.h"
#include "CTurret.h"

#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

CTurret::CTurret()
    : CStructure(SCRIPT_TYPE::TURRET)
{
}

CTurret::~CTurret()
{
}

void CTurret::Begin()
{
    CStructure::Begin();

    Ptr<CCollider2D> AttackCollider = GetOwner()->GetChild(0)->GetCollider2D();
    if (!AttackCollider)
    {
        DebugUtil::AddDebugLog("[CTurret::Begin] : Cannot find Child AttackCollider!", DEF_COLOR_RED, 20.f);
        return;
    }
    AttackCollider->AddDynamicBeginOverlap(this, static_cast<COLLISION_EVENT>(&CTurret::AttackColliderBeginOverlap));
    AttackCollider->AddDynamicEndOverlap(this, static_cast<COLLISION_EVENT>(&CTurret::AttackColliderEndOverlap));
}

void CTurret::Tick()
{
    CStructure::Tick();
}

void CTurret::AttackColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // Enemy 종류가 아닌 경우 무시
    if (!_OtherCollider->GetOwner()->GetScriptComponent<CEnemyScript>()) return;
    
    m_setAttackRangeEnteredEnemies.insert(_OtherCollider->GetOwner());
    
    DebugUtil::AddDebugLog(GetOwner()->GetName() + L" : Detected Enemy count : " + to_wstring(m_setAttackRangeEnteredEnemies.size()), DEF_COLOR_CYAN, 10.f);
    
}

void CTurret::AttackColliderEndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // Enemy 종류가 아닌 경우 무시
    if (!_OtherCollider->GetOwner()->GetScriptComponent<CEnemyScript>()) return;
    
    m_setAttackRangeEnteredEnemies.erase(_OtherCollider->GetOwner());
    
    // 공격 시도 또는 공격하는 중이었던 TargetObject가 반경에서 벗어난 상황,
    // 다시 공격할 대상 재탐색
    if (m_TargetEnemyObject == _OtherCollider->GetOwner())
        m_CurrentTurretState = TURRET_STATE::IDLE;
    DebugUtil::AddDebugLog(GetOwner()->GetName() + L" : Detected Enemy count : " + to_wstring(m_setAttackRangeEnteredEnemies.size()), DEF_COLOR_CYAN, 10.f);
}
