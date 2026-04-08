#include "pch.h"
#include "CDevil.h"

#include "Source/ScriptMgr.h"

CDevil::CDevil()
    : CEnemyScript(SCRIPT_TYPE::DEVIL)
{
}

CDevil::~CDevil()
{
}

void CDevil::Begin()
{
    CEnemyScript::Begin();

    // Attack Notify를 받지 않음(해당 함수에서는 CPerceptionHandler가 들고 있는 AttackDamage 반경 Collider를 켜고 끄는 처리를 한다)
    for (int i = 0; i < 8; ++i) FlipbookRender()->RemoveNotifyFlipbookOnSpriteIdx(L"Attack", i);
        
}

void CDevil::Tick()
{
    CEnemyScript::Tick();
}

void CDevil::OnAttackFlipbookEndNotify()
{
    CEnemyScript::OnAttackFlipbookEndNotify();
}

void CDevil::HandleStateTransition()
{
}
