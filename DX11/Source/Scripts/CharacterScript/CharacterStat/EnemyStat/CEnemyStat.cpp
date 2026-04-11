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
    const Ptr<CEnemyScript>& MainEnemyScript = GetOwner()->GetScriptComponent<CEnemyScript>();
    
    // 만약 First Spawn 처리 중이고, 아직 Boundary를 넘지 못하였을 때 (적어도 맵에 들어온 뒤에야 피격 가능하도록 처리)
    if (MainEnemyScript->GetCurrentWalkType() == ENEMY_WALK_TYPE::FIRST_SPAWN_WALK && MainEnemyScript->IsCurrentlyOutOfBound()) return false;
    
    if (!CCharacterStat::TakeDamage(_DamageAmount, _DamageCauser)) return false;

    
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
