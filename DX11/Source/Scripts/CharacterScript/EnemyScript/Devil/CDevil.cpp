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
