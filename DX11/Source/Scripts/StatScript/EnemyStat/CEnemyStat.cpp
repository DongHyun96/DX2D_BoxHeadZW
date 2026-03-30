#include "pch.h"
#include "CEnemyStat.h"

#include "Source/ScriptMgr.h"

CEnemyStat::CEnemyStat()
    : CStatScript(SCRIPT_TYPE::ENEMYSTAT)
{
}

CEnemyStat::~CEnemyStat()
{
}

bool CEnemyStat::TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos)
{
    if (!CStatScript::TakeDamage(_DamageAmount, _DamageSourcePos)) return false;

    // 사망 체크할 것
    // TODO : 사망 시 사망 Animation 재생 및 사망 끝 처리 EndEvent에 다시 Pool로 돌아가게끔 처리할 것
    
    return true;
}
