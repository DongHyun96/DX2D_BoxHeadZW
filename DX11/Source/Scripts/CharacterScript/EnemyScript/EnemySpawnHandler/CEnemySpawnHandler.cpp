#include "pch.h"
#include "CEnemySpawnHandler.h"

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/EnemyScript/Mummy/CMummy.h"
#include "Source/Scripts/CharacterScript/EnemyScript/PerceptionHandler/CPerceptionHandler.h"
#include "Source/Scripts/FirstSpawnLocManager/CFirstSpawnLocManager.h"
#include "Source/Scripts/UIScript/CText.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"


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
    GM->GetIngameUIManager()->GetZombieAliveCount()->SetText(to_wstring(m_SpawnedCount));
}

GameObject* CEnemySpawnHandler::SpawnEnemyOnFirstSpawnArea(ENEMY_TYPE _EnemyType, FIRST_SPAWN_LOC _SpawnLoc)
{
    if (_SpawnLoc == FIRST_SPAWN_LOC_END)                 return nullptr;
    if (!m_mapEnemyPoolers[_EnemyType]->CanSpawnObject()) return nullptr;

    GameObject* SpawnedEnemy = m_mapEnemyPoolers[_EnemyType]->SpawnObject(false);
    if (!SpawnedEnemy) return nullptr;

    ++m_SpawnedCount;
    
    CEnemyScript* EnemyScript = SpawnedEnemy->GetScriptComponent<CEnemyScript>().Get();
    
    // Spawn Area에서의 랜덤한 영역 뽑기
    CTransform* SpawnAreaTransform = GM->GetFirstSpawnLocManager()->GetFirstSpawnAreaTransform(_SpawnLoc);
    
    const Vec2 AreaPos             = SpawnAreaTransform->GetRelativePosXY();
    const Vec2 SpawnPossibleSize   = (SpawnAreaTransform->GetRelativeScaleXY() - EnemyScript->GetBodySize()) * 0.5f;
    
    const Vec2 PickedPos = AreaPos + Vec2(GetRandom(-SpawnPossibleSize.x, SpawnPossibleSize.x),
                                          GetRandom(-SpawnPossibleSize.y, SpawnPossibleSize.y));

    SpawnedEnemy->Transform()->SetRelativePosXY(PickedPos);
    
    const CellCoord& PickedRandomDestCellCoord = GM->GetBackgroundCellManager()->GetRandomFirstSpawnLocDestination(_SpawnLoc);
    
    TryInitSpawnedEnemy(EnemyScript);
    EnemyScript->SetCurrentWalkType(ENEMY_WALK_TYPE::FIRST_SPAWN_WALK);
    EnemyScript->SetFirstSpawnMoveDestination(PickedRandomDestCellCoord);
    
    return SpawnedEnemy;
}

GameObject* CEnemySpawnHandler::SpawnEnemyOnAvailableCell(ENEMY_TYPE _EnemyType, const Vec2& _SpawnPos)
{
    return SpawnEnemyOnAvailableCell(_EnemyType, GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(_SpawnPos));
}

GameObject* CEnemySpawnHandler::SpawnEnemyOnAvailableCell(ENEMY_TYPE _EnemyType, const CellCoord& _CellCoord)
{
    // Available Cell인지 조사
    if (!GM->GetBackgroundCellManager()->IsCellAvailable(_CellCoord)) return nullptr;    
    
    // Player와 Adjacent한 Cell인지 조사
    const Vec2 PlayerPos      = GM->GetPlayerObject()->Transform()->GetRelativePosXY();
    CellCoord PlayerCellCoord = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(PlayerPos);
    
    if (PlayerCellCoord.x - 1 <= _CellCoord.x && _CellCoord.x <= PlayerCellCoord.x + 1) return nullptr;
    if (PlayerCellCoord.y - 1 <= _CellCoord.y && _CellCoord.y <= PlayerCellCoord.y + 1) return nullptr;
    
    GameObject* SpawnedEnemy = m_mapEnemyPoolers[_EnemyType]->SpawnObject(ToVec3(GM->GetBackgroundCellManager()->GetCellCoordToWorldPos(_CellCoord)), false);
    
    if (SpawnedEnemy)
    {
        CEnemyScript* EnemyScript = SpawnedEnemy->GetScriptComponent<CEnemyScript>().Get();
        TryInitSpawnedEnemy(EnemyScript);
        EnemyScript->SetCurrentWalkType(ENEMY_WALK_TYPE::FIRST_SPAWN_WALK);
        ++m_SpawnedCount;
    }
    
    return SpawnedEnemy; 
}

GameObject* CEnemySpawnHandler::SpawnEnemyOnRandomCell(ENEMY_TYPE _EnemyType)
{
    return SpawnEnemyOnAvailableCell(_EnemyType, CellCoord(GetRandom(25, 55), GetRandom(25, 55)));
}

/*void CEnemySpawnHandler::TryInitSpawnedEnemy(GameObject* EnemyObject)
{
    if (EnemyObject) TryInitSpawnedEnemy(EnemyObject->GetScriptComponent<CEnemyScript>().Get());
}*/

void CEnemySpawnHandler::TryInitSpawnedEnemy(CEnemyScript* _EnemyScript)
{
    if (!_EnemyScript) return;
    _EnemyScript->InitSpawn();

    if (CMummy* Mummy = dynamic_cast<CMummy*>(_EnemyScript))
        Mummy->SetSpawnedByMummy(false);
}
