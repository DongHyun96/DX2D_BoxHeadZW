#include "pch.h"
#include "CFirstSpawnLocManager.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CFirstSpawnLocManager::CFirstSpawnLocManager()
    : CScript(SCRIPT_TYPE::FIRSTSPAWNLOCMANAGER)
{
}

CFirstSpawnLocManager::~CFirstSpawnLocManager()
{
}

void CFirstSpawnLocManager::Begin()
{
    GM->SetFirstSpawnLocManager(this);
    
    // Init First Spawn areas
    for (int i = 0; i < FIRST_SPAWN_LOC_END; ++i)
    {
        const wstring ChildObjectName = L"EnemySpawnArea" + to_wstring(i + 1);
        Ptr<GameObject> SpawnAreaObject = GetOwner()->GetChildByName(ChildObjectName);
        m_mapFirstSpawnAreaTransform[static_cast<FIRST_SPAWN_LOC>(i)] = SpawnAreaObject->Transform().Get();
    }
}

void CFirstSpawnLocManager::Tick()
{
}
