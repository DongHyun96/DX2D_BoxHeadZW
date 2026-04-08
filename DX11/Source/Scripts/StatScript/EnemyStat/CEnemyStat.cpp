#include "pch.h"
#include "CEnemyStat.h"

#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

CEnemyStat::CEnemyStat()
    : CCharacterStat(SCRIPT_TYPE::ENEMYSTAT)
{
}

CEnemyStat::~CEnemyStat()
{
}

bool CEnemyStat::TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos)
{
    if (!CCharacterStat::TakeDamage(_DamageAmount, _DamageSourcePos)) return false;

    // 사망 체크할 것
    // TODO : 사망 시 사망 Animation 재생 및 사망 끝 처리 EndEvent에 다시 Pool로 돌아가게끔 처리할 것 -> 이거를 Dead Flipbook 이후
    // EndEvent Callback으로 걸어주어야 함
    
    const ENEMY_MAINSTATE NextState = IsDead() ? ENEMY_MAINSTATE::DIE : ENEMY_MAINSTATE::PUSHED_OUT;
    const Ptr<CEnemyScript>& MainEnemyScript = GetOwner()->GetScriptComponent<CEnemyScript>();
    
    MainEnemyScript->SetMainState(NextState);
    
    GetOwner()->GetScriptComponent<CEnemyScript>()->OnTakeDamage();
    
    return true;
}
