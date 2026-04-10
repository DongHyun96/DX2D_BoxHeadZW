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

bool CEnemyStat::TakeDamage(float _DamageAmount, GameObject* _DamageCauser)
{
    if (!CCharacterStat::TakeDamage(_DamageAmount, _DamageCauser)) return false;

    const Ptr<CEnemyScript>& MainEnemyScript = GetOwner()->GetScriptComponent<CEnemyScript>();
    
    // 사망 체크
    if (IsDead())
    {
        MainEnemyScript->SetMainState(ENEMY_MAINSTATE::DIE);
        MainEnemyScript->OnDieStart();
    }
    else
        MainEnemyScript->SetMainState(ENEMY_MAINSTATE::PUSHED_OUT);
    
    MainEnemyScript->OnTakeDamage(_DamageCauser);
    return true;
}
