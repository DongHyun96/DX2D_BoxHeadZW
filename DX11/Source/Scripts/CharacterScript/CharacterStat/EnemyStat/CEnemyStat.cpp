#include "pch.h"
#include "CEnemyStat.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"
#include "Source/Scripts/Item/CItem.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"

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
        
        if (GetRandom(0.f, 1.f) < 0.4f)
        {
            GameObject* Object = GM->GetItemPooler()->SpawnObject(GetOwner()->Transform()->GetRelativePos());
            if (Object)
            {
                // 아이템이 확실히 보이도록 Default(0) 또는 Tile(2) 레이어로 설정 시도
                // (프리팹 기본 설정이 잘못되어 있을 가능성 대비)
                Object->SetLayerIdx(0); 
            }
        }
    }
    else
        MainEnemyScript->SetMainState(ENEMY_MAINSTATE::PUSHED_OUT);
    
    MainEnemyScript->OnTakeDamage(_DamageCauser);
    
    
    return true;
}
