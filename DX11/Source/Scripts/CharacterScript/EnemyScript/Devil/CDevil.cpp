#include "pch.h"
#include "CDevil.h"

#include "Source/ScriptMgr.h"
#include "CFlameLineHandler.h"

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

    m_FlameLineHandler = GetOwner()->GetChild(0)->GetScriptComponent<CFlameLineHandler>().Get();
    
    for (int i = 0; i < 8; ++i)
    {
        // Sprite 중간 지점에서의 Attack Notify를 받지 않음(해당 함수에서는 CPerceptionHandler가 들고 있는 AttackDamage 반경 Collider를 켜고 끄는 처리를 한다)
        FlipbookRender()->RemoveNotifyFlipbookOnSpriteIdx(L"Attack", i);
        
        // 대신 Attack Flipbook 시작 시, 불쏘시개 Effect 스폰 처리 함수
        FlipbookRender()->AddNotifyFlipbookStartEvent(L"Attack", i, bind(&CDevil::OnAttackAnimStart, this));
    }
}

void CDevil::Tick()
{
    CEnemyScript::Tick();
}

void CDevil::OnTakeDamage()
{
    CEnemyScript::OnTakeDamage();
}

void CDevil::HandleStateTransition()
{
    switch (m_MainState)
    {
    case ENEMY_MAINSTATE::ATTACK: case ENEMY_MAINSTATE::PUSHED_OUT: case ENEMY_MAINSTATE::DIE: case ENEMY_MAINSTATE::END: return;
        
    case ENEMY_MAINSTATE::WALK:
    {
        // TODO : Target Object가 없다면 TargetObject 찾기 
    }
        return;
    }
}

void CDevil::OnAttackFlipbookEndNotify()
{
    CEnemyScript::OnAttackFlipbookEndNotify();
}

void CDevil::OnAttackAnimStart()
{
    
}
