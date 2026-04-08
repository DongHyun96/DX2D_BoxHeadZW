#include "pch.h"
#include "CEnemySpawnHandler.h"

#include "Source/ScriptMgr.h"


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
}

void CEnemySpawnHandler::Tick()
{
}

