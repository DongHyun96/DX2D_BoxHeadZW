#include "pch.h"
#include "CEnemySpawnHandler.h"

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/EnemyScript/Mummy/CMummy.h"
#include "Source/Scripts/CharacterScript/EnemyScript/PerceptionHandler/CPerceptionHandler.h"


CEnemySpawnHandler::CEnemySpawnHandler()
    : CScript(SCRIPT_TYPE::ENEMYSPAWNHANDLER)
{
}

CEnemySpawnHandler::~CEnemySpawnHandler()
{
}

void CEnemySpawnHandler::Begin()
{
    // Init poolers
    for (const Ptr<GameObject>& Child : GetOwner()->GetChildren())
    {
        if (Child->GetName() == L"ZombiePooler")        m_mapEnemyPoolers[ENEMY_TYPE::ZOMBIE]   = Child->PoolComponent().Get();
        else if (Child->GetName() == L"MummyPooler")    m_mapEnemyPoolers[ENEMY_TYPE::MUMMY]    = Child->PoolComponent().Get();
        else if (Child->GetName() == L"RunnerPooler")   m_mapEnemyPoolers[ENEMY_TYPE::RUNNER]   = Child->PoolComponent().Get();
        else if (Child->GetName() == L"VampirePooler")  m_mapEnemyPoolers[ENEMY_TYPE::VAMPIRE]  = Child->PoolComponent().Get();
        else if (Child->GetName() == L"DevilPooler")    m_mapEnemyPoolers[ENEMY_TYPE::DEVIL]    = Child->PoolComponent().Get();
    }
    
    GM->SetEnemySpawnHandler(this);
}

void CEnemySpawnHandler::Tick()
{
    if (KEY_TAP(KEY::MRB))
    {
        /*for (int i = 0; i < 50; ++i)
        {
            ENEMY_TYPE Type = static_cast<ENEMY_TYPE>(GetRandom(0, 3));
            SpawnEnemy(Type, CellCoord(GetRandom(25, 55), GetRandom(25, 55)));
        }*/
        for (int i = 0; i < 5; ++i)
			    SpawnEnemy(ENEMY_TYPE::ZOMBIE, CellCoord(GetRandom(25, 55), GetRandom(25, 55)));
    }
}

GameObject* CEnemySpawnHandler::SpawnEnemy(ENEMY_TYPE _EnemyType, const Vec2& _SpawnPos)
{
    return SpawnEnemy(_EnemyType, GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(_SpawnPos));
}

GameObject* CEnemySpawnHandler::SpawnEnemy(ENEMY_TYPE _EnemyType, const CellCoord& _CellCoord)
{
    // Available Cell인지 조사 TODO : (근데 맨 처음 시작 시에는, 맵 밖에서 출발을 해야 함) -> 이거는 초반에 Tick에서 처리를 좀 해야할듯
    if (!GM->GetBackgroundCellManager()->IsCellAvailable(_CellCoord)) return nullptr;    
    
    // Player와 Adjacent한 Cell인지 조사
    const Vec2 PlayerPos      = GM->GetPlayerObject()->Transform()->GetRelativePosXY();
    CellCoord PlayerCellCoord = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(PlayerPos);
    
    if (PlayerCellCoord.x - 1 <= _CellCoord.x && _CellCoord.x <= PlayerCellCoord.x + 1) return nullptr;
    if (PlayerCellCoord.y - 1 <= _CellCoord.y && _CellCoord.y <= PlayerCellCoord.y + 1) return nullptr;
    
    GameObject* SpawnedEnemy = m_mapEnemyPoolers[_EnemyType]->SpawnObject(ToVec3(GM->GetBackgroundCellManager()->GetCellCoordToWorldPos(_CellCoord)), false);
    if (SpawnedEnemy)
    {
        CEnemyScript* Enemy = SpawnedEnemy->GetScriptComponent<CEnemyScript>().Get();
        Enemy->InitSpawn();

        if (CMummy* Mummy = dynamic_cast<CMummy*>(Enemy))
            Mummy->SetSpawnedByMummy(false);
    }
    
    return SpawnedEnemy; 
}
