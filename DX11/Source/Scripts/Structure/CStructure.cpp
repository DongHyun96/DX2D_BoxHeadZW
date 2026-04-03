#include "pch.h"
#include "CStructure.h"

#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/CCharacterScript.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CStructure::CStructure()
    : CScript(SCRIPT_TYPE::STRUCTURE)
{
}

CStructure::~CStructure()
{
}

CStructure::CStructure(SCRIPT_TYPE _ScriptType)
    : CScript(_ScriptType)
{
}

void CStructure::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CStructure::BeginOverlap);
    ADD_DYNAMIC_OVERLAP(CStructure::Overlap);
}

void CStructure::Tick()
{
}

void CStructure::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // Character Movement로 해당 Collider에 부딪힌 경우
    if (BlockCharacterCollider(_OtherCollider)) return;
    
    // 나머지는 Enemy Attack Area -> TakeDamage 처리는 각자의 Projectile 및 AttackArea Overlap에서 처리할 것
    // Barrel의 경우, 예외적으로 PlayerProjectile으로도 Damage를 입어야 한다.
    // 근데 Damage 량을 알아야 함
    // GetOwner()->GetScriptComponent<CStatScript>()->TakeDamage()
}

void CStructure::Overlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    if (BlockCharacterCollider(_OtherCollider)) return;
}

bool CStructure::BlockCharacterCollider(CCollider2D* _OtherCollider)
{
    // 프리뷰 오브젝트에 캐릭터가 blocking되면 안됨
    if (m_IsPreviewObject) return false;
    
    // 캐릭터가 아닌 다른 GameObject
    if (!_OtherCollider->GetOwner()->GetScriptComponent<CCharacterScript>()) return false;

    _OtherCollider->Transform()->UpdateTransformToPrevRelativePos();
    return true;
}

